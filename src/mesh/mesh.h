//
// Created by juaquin on 5/7/24.
//

#ifndef RAYTRACING_WEEKEND_MESH_H
#define RAYTRACING_WEEKEND_MESH_H

#include "../hittable/hitabble.h"
#include "../rtweekend.h"
#include "../triangle/triangle.h"
#include <vector>

class mesh: public hittable {
private:
    std::vector<shared_ptr<triangle>> data;
public:
    mesh(const std::initializer_list<shared_ptr<triangle>>& data) {
        this->data.assign(data);
    }
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
// TODO
        return false;
    }
};

#endif //RAYTRACING_WEEKEND_MESH_H
