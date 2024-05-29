//
// Created by juaquin on 5/27/24.
//

#ifndef RAYTRACING_WEEKEND_SPHERE_H
#define RAYTRACING_WEEKEND_SPHERE_H

#include "primitive.h"
#include "../rtweekend.h"
#include "../material/material.h"
#include <array>

class sphere: public primitive{
public:
    point3 center;
    float radius;
    shared_ptr<material> mat;

    sphere(const point3& center, double radius, const shared_ptr<material>& mat): radius(fmax(0, radius)),
                                                                                  mat(mat){}

    void set_material(hit_record& rec, const unsigned int& primID, const float& u, const float& v) override {
        rec.mat = this->mat;
    }

    std::array<float, 4> get_sphere() {
        return {static_cast<float>(center.x()), static_cast<float>(center.y()), static_cast<float>(center.z()), static_cast<float>(radius)};
    }
};

#endif //RAYTRACING_WEEKEND_SPHERE_H
