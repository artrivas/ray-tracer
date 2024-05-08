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

class mesh: public hittable {
private:
public:
    mesh() = default;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        return false;
    }

    static mesh build(const std::string& filename) {
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

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
};

#endif //RAYTRACING_WEEKEND_MESH_H
