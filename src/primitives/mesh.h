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
    shared_ptr<material> mat = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    point3 origin{0, 0, 0};
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<int> materials_id;

    // BVH
    shared_ptr<bvhTree> bvh;

    void dump_material_ids(vector<int>& mat_ids) {
        for (auto& shape: shapes) {
            for (auto& id: shape.mesh.material_ids) {
                mat_ids.emplace_back(id);
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
        size_t prim_id = this->bvh->hit(ray, rec);

        if (prim_id != std::numeric_limits<size_t>::max()) {
            auto t = bvh->precomputed.at(prim_id);
            rec.t = ray.tmax;
            auto material_id = this->materials_id.at(prim_id);
            if (material_id == -1) {
                rec.mat = make_shared<metal>(color(1, 0.4, 0.3), 0.5);
            }
            else {
                auto c = materials.at(material_id).diffuse;
                rec.mat = make_shared<lambertian>(color(c[0], c[1], c[2]));
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
            std::clog << "shapes: " << new_mesh.shapes.size() << std::endl;
            std::clog << "Mesh loaded: " << new_mesh.shapes[0].name << " - " << new_mesh.attrib.vertices.size() <<
            std::endl;
            cout << new_mesh.materials.size() << endl;
            cout << new_mesh.attrib.normals.size() << endl;
        }
        std::vector<Tri> tris;
        auto lm = [&tris, new_mesh](const point3 &vtx1, const point3 &vtx2, const point3 &vtx3) {
            tris.emplace_back(convert::to(vtx1), convert::to(vtx2), convert::to(vtx3));
        };
        // fill tris with the data
        new_mesh.for_each(lm);

        // Prepare to have the material_id
        new_mesh.materials_id.resize(tris.size());

        // Use for reorder the materials id
        vector<int> temp_materials;
        new_mesh.dump_material_ids(temp_materials);
        auto lm1 = [&new_mesh, temp_materials](size_t& i, size_t& j) {
            new_mesh.materials_id[i] = temp_materials[j];
        };
        new_mesh.bvh = make_shared<bvhTree>(tris, lm1);

        return std::move(new_mesh);
    }
};

#endif //RAYTRACING_WEEKEND_MESH_H
