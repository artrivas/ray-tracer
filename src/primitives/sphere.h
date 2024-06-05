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

    sphere(const point3& center, float radius, const shared_ptr<material>& mat): center(center), radius(fmax(0, radius)),
                                                                                  mat(mat){}

    void set_material(hit_record& rec, const unsigned int& primID, const float& u, const float& v) override {
        auto p = (rec.p - center)/radius;

        auto theta = asin(- p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;
        rec.u = (phi) / (2*pi);
        rec.v = theta / (pi);

        rec.u = 0.01;
        rec.v = 0.02;
        rec.mat = this->mat;
    }

    std::array<float, 4> get_sphere() {
        return {static_cast<float>(center.x()), static_cast<float>(center.y()), static_cast<float>(center.z()), static_cast<float>(radius)};
    }
};

#endif //RAYTRACING_WEEKEND_SPHERE_H
