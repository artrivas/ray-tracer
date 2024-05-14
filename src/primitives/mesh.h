//
// Created by juaquin on 5/7/24.
//

#ifndef RAYTRACING_WEEKEND_MESH_H
#define RAYTRACING_WEEKEND_MESH_H

#define TINYOBJLOADER_IMPLEMENTATION

#include "../hittable/hitabble.h"
#include "../rtweekend.h"
#include <vector>
#include <filesystem>
#include "../ext/tiny_obj_loader/tiny_obj_loader.h"
#include "../primitives/triangle.h"
#include "../material/material.h"
#include "../accelerator/bvh.h"



class mesh : public hittable {
private:
    struct metadata_triangle {
        tinyobj::index_t* vertex[3];
        int material_id;

        metadata_triangle(tinyobj::index_t* v1, tinyobj::index_t* v2, tinyobj::index_t* v3, const int mat_id):
        material_id(mat_id) {
            vertex[0] = v1;
            vertex[1] = v2;
            vertex[2] = v3;
        }
    };

    shared_ptr<material> mat = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    point3 origin{0, 0, 0};
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<metadata_triangle> meta;

    // BVH
    shared_ptr<bvhTree> bvh;


    void initialize() {
        meta.clear();
        for (auto& shape: shapes) {
            for (int i = 0; i < shape.mesh.indices.size()/3; i++) {
                meta.emplace_back(&shape.mesh.indices.at(3*i), &shape.mesh.indices.at(3*i+1), &shape.mesh.indices.at
                (3*i+2), shape.mesh.material_ids.at(i));
            }
        }
    }

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
        float u, v;
        size_t prim_id = this->bvh->hit(ray, rec, u, v);

        if (prim_id != std::numeric_limits<size_t>::max()) {
            auto t = bvh->precomputed.at(prim_id);
            rec.t = ray.tmax;
            // Get data about the intersected triangle
            auto triangle_data = this->meta.at(this->bvh->bvh->prim_ids[prim_id]);
            if (triangle_data.vertex[0]->texcoord_index != -1) {
                // Texture Mapping
//                float z = 1 - u - v;
//                attrib.texcoords.at(triangle_data.vertex[0]->texcoord_index);
//                attrib.texcoords.at(triangle_data.vertex[1]->texcoord_index);
//                attrib.texcoords.at(triangle_data.vertex[2]->texcoord_index);
            }
            else if (triangle_data.material_id != -1) {
                auto c = materials.at(triangle_data.material_id).diffuse;
                rec.mat = make_shared<lambertian>(color(c[0], c[1], c[2]));
            }
            else {
                rec.mat = make_shared<dielectric>(0.5);
            }
            rec.p = r.at(rec.t);
            vec3 outward_normal = convert::from(normalize(t.n));
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
            auto mtl_dir = filesystem::path(filename).parent_path().c_str();
            bool status = tinyobj::LoadObj(&new_mesh.attrib, &new_mesh.shapes, &new_mesh.materials, nullptr, &err,
                                           filename
                                                   .c_str(), mtl_dir);
            if (!err.empty()) {
                std::cerr << "Failed to load: " << err << std::endl;
                exit(1);
            }
            if (!status) exit(1);

            clog << "Loaded OBJ" << endl;
            std::clog << "shapes: " << new_mesh.shapes.size() << std::endl;
            for (auto& shape: new_mesh.shapes) {
                clog << shape.name << endl;
                clog << "\ttriangles: " << shape.mesh.indices.size()/3 << endl;
            }
            clog << "materials: " << new_mesh.materials.size() << endl;
        }
        std::vector<Tri> tris;
        auto lm = [&tris, new_mesh](const point3 &vtx1, const point3 &vtx2, const point3 &vtx3) {
            tris.emplace_back(convert::to(vtx1), convert::to(vtx2), convert::to(vtx3));
        };
        // fill tris with the data
        new_mesh.for_each(lm);

        // Prepare to have the material_id
        new_mesh.initialize();
        new_mesh.bvh = make_shared<bvhTree>(tris);

        return std::move(new_mesh);
    }
};

#endif //RAYTRACING_WEEKEND_MESH_H
