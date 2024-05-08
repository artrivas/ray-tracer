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

class mesh : public hittable {
private:
public:
    mesh() = default;

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        bool intersects = false;

        for (int i = 0; i < this->shapes[0].mesh.indices.size() / 3; i++) {
            int idx0 = shapes[0].mesh.indices.at(3 * i).vertex_index;
            int idx1 = shapes[0].mesh.indices.at(3 * i + 1).vertex_index;
            int idx2 = shapes[0].mesh.indices.at(3 * i + 2).vertex_index;

            point3 vtx1{attrib.vertices.at(3 * idx0), attrib.vertices.at(3 * idx0 + 1),
                        attrib.vertices.at(3 * idx0 + 2)};
            point3 vtx2{attrib.vertices.at(3 * idx1), attrib.vertices.at(3 * idx1 + 1),
                        attrib.vertices.at(3 * idx1 + 2)};
            point3 vtx3{attrib.vertices.at(3 * idx2), attrib.vertices.at(3 * idx2 + 1),
                        attrib.vertices.at(3 * idx2 + 2)};

            triangle t(vtx1,vtx2,vtx3, this->mat);

            if (t.hit(r, ray_t, rec)) {
                intersects = true;
            }
        }
        return intersects;
    }

    [[nodiscard]] point3 get_origin() const {
        return this->origin;
    }

    void set_origin(const point3& _origin) {
        for (int i = 0; i < this->attrib.vertices.size()/3; i++) {
            this->attrib.vertices.at(3*i + 0) += _origin.x() - this->origin.x();
            this->attrib.vertices.at(3*i + 1) += _origin.y() - this->origin.y();
            this->attrib.vertices.at(3*i + 2) += _origin.z() - this->origin.z();
        }
        this->origin = _origin;
    }

    static mesh build(const std::string &filename) {
        mesh new_mesh;
        std::string err;
        bool status = tinyobj::LoadObj(&new_mesh.attrib, &new_mesh.shapes, &new_mesh.materials, nullptr, &err, filename
                .data());
        if (!err.empty()) {
            std::cerr << "Failed to load: " << err << std::endl;
            exit(1);
        }
        if (!status) exit(1);

        return std::move(new_mesh);
    }

    shared_ptr<material> mat = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    point3 origin{0, 0, 0};
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
};

#endif //RAYTRACING_WEEKEND_MESH_H
