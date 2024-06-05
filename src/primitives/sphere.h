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
    //17220108
    shared_ptr<material> mat;

    sphere(const point3& center, float radius, const shared_ptr<material>& mat): center(center), radius(radius),
                                                                                  mat(mat){}

    void set_material(hit_record& rec, const unsigned int& primID, const float& u, const float& v) override {
        auto p = unit_vector((rec.p - center));
        rec.u = atan2(p.x(), p.z()) / (2*std::numbers::pi) + 0.5;
        rec.v = p.y()*0.5 + 0.5;

        rec.mat = this->mat;
    }

    std::array<float, 4> get_sphere() const {
        return {static_cast<float>(center.x()), static_cast<float>(center.y()), static_cast<float>(center.z()), static_cast<float>(radius)};
    }
};

#endif //RAYTRACING_WEEKEND_SPHERE_H
