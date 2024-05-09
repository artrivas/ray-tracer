//
// Created by juaquin on 5/7/24.
//

#ifndef RAYTRACING_WEEKEND_MESH_H
#define RAYTRACING_WEEKEND_MESH_H

#define TINYOBJLOADER_IMPLEMENTATION

#include "../hittable/hitabble.h"
#include "../rtweekend.h"
#include "triangle.h"
#include <vector>
#include "../ext/tiny_obj_loader/tiny_obj_loader.h"
#include "../primitives/triangle.h"
#include "../material/material.h"
#include "bvh/v2/bvh.h"
#include "bvh/v2/executor.h"
#include "bvh/v2/thread_pool.h"
#include "bvh/v2/bbox.h"
#include "bvh/v2/tri.h"
#include "bvh/v2/default_builder.h"
#include "bvh/v2/stack.h"

using Scalar = float;
using Vec3 = bvh::v2::Vec<Scalar, 3>;
using BBox = bvh::v2::BBox<Scalar, 3>;
using Tri = bvh::v2::Tri<Scalar, 3>;
using Node = bvh::v2::Node<Scalar, 3>;
using Bvh = bvh::v2::Bvh<Node>;
using Ray = bvh::v2::Ray<Scalar, 3>;


using PrecomputedTri = bvh::v2::PrecomputedTri<Scalar>;

namespace convert {
    Vec3 to(const vec3 &v) {
        return {static_cast<float>(v.x()), static_cast<float>(v.y()), v.z()};
    }

    vec3 from(const Vec3 &v) {
        return {v.values[0], v.values[1], v.values[2]};
    }
}

class mesh : public hittable {
private:
    shared_ptr<material> mat = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    point3 origin{0, 0, 0};
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    // BVH
    shared_ptr<Bvh> bvh;
    std::vector<PrecomputedTri> precomputed;
public:
    mesh() = default;

    void for_each(auto fn) {
        for (auto& shape : shapes) {
            for (int i = 0; i < shape.mesh.indices.size() / 3; i++) {
                const int &idx0 = shape.mesh.indices.at(3 * i).vertex_index;
                const int &idx1 = shape.mesh.indices.at(3 * i + 1).vertex_index;
                const int &idx2 = shape.mesh.indices.at(3 * i + 2).vertex_index;
                point3 vtx1{attrib.vertices.at(3 * idx0), attrib.vertices.at(3 * idx0 + 1),
                            attrib.vertices.at(3 * idx0 + 2)};
                point3 vtx2{attrib.vertices.at(3 * idx1), attrib.vertices.at(3 * idx1 + 1),
                            attrib.vertices.at(3 * idx1 + 2)};
                point3 vtx3{attrib.vertices.at(3 * idx2), attrib.vertices.at(3 * idx2 + 1),
                            attrib.vertices.at(3 * idx2 + 2)};
                vtx1 += origin;
                vtx2 += origin;
                vtx3 += origin;
                fn(vtx1, vtx2, vtx3);
            }
        }
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        auto ray = Ray{
                convert::to(r.origin()),
                convert::to(r.direction()),
                static_cast<float>(ray_t.min),
                static_cast<float>(ray_t.max),
        };

        static constexpr size_t invalid_id = std::numeric_limits<size_t>::max();
        static constexpr size_t stack_size = 64;
        static constexpr bool use_robust_traversal = false;

        auto prim_id = invalid_id;
        Scalar u, v;
        static constexpr bool should_permute = true;

        bvh::v2::SmallStack<Bvh::Index, stack_size> stack;
        bvh->intersect<false, use_robust_traversal>(ray, bvh->get_root().index, stack,
                                                   [&](size_t begin, size_t end) {
                                                       for (size_t i = begin; i < end; ++i) {
                                                           size_t j = should_permute ? i : bvh->prim_ids[i];
                                                           if (auto hit = precomputed[j].intersect(ray)) {
                                                               prim_id = i;
                                                               std::tie(u, v) = *hit;
                                                           }
                                                       }
                                                       return prim_id != invalid_id;
                                                   });

        if (prim_id != invalid_id) {
            auto t = precomputed.at(prim_id);
            rec.t = ray.tmax;
            rec.mat = this->mat;
            rec.p = r.at(rec.t);
            vec3 outward_normal = convert::from(normalize(cross(t.e1, t.e2)));
            rec.set_face_normal(r, outward_normal);
            return true;
        } else return false;
    }

    static mesh build(const std::string &filename, const point3& origin) {
        mesh new_mesh;
        new_mesh.origin = origin;
        {
            // READ OBJ FILE
            std::string err;
            bool status = tinyobj::LoadObj(&new_mesh.attrib, &new_mesh.shapes, &new_mesh.materials, nullptr, &err,
                                           filename
                                                   .data());
            if (!err.empty()) {
                std::cerr << "Failed to load: " << err << std::endl;
                exit(1);
            }
            if (!status) exit(1);
            std::clog << "shapes: " << new_mesh.shapes.size() << std::endl;
            std::clog << "Mesh loaded: " << new_mesh.shapes[0].name << " - " << new_mesh.attrib.vertices.size() <<
            std::endl;
        }

        std::vector<Tri> tris;
        auto lm = [&tris](const point3 &vtx1, const point3 &vtx2, const point3 &vtx3) {
            tris.emplace_back(Vec3(vtx1.x(), vtx1.y(), vtx1.z()), Vec3(vtx2.x(), vtx2.y(), vtx2.z()), Vec3(vtx3.x(),
                                                                                                           vtx3.y(),
                                                                                                           vtx3.z()));
        };
        // fill tris with the data
        new_mesh.for_each(lm);

        bvh::v2::ThreadPool thread_pool;
        bvh::v2::ParallelExecutor executor(thread_pool);

        std::vector<BBox> bboxes(tris.size());
        std::vector<Vec3> centers(tris.size());

        executor.for_each(0, tris.size(), [&](size_t begin, size_t end) {
            for (size_t i = begin; i < end; ++i) {
                bboxes[i] = tris[i].get_bbox();
                centers[i] = tris[i].get_center();
            }
        });

        typename bvh::v2::DefaultBuilder<Node>::Config config;
        config.quality = bvh::v2::DefaultBuilder<Node>::Quality::High;
        new_mesh.bvh = make_shared<Bvh>(bvh::v2::DefaultBuilder<Node>::build(thread_pool, bboxes, centers, config));

        static constexpr bool should_permute = true;

        new_mesh.precomputed.resize(tris.size());
        executor.for_each(0, tris.size(), [&](size_t begin, size_t end) {
            for (size_t i = begin; i < end; ++i) {
                auto j = should_permute ? new_mesh.bvh->prim_ids[i] : i;
                new_mesh.precomputed[i] = tris[j];
            }
        });
        return std::move(new_mesh);
    }
};

#endif //RAYTRACING_WEEKEND_MESH_H
