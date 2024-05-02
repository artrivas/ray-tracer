//
// Created by USER on 5/2/2024.
//

#ifndef RAYTRACING_WEEKEND_HITABBLE_H
#define RAYTRACING_WEEKEND_HITABBLE_H

#include "../ray/ray.h"

class hit_record {
public:
    point3  p;
    vec3 normal;
    double t;
};

class hittable {
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif //RAYTRACING_WEEKEND_HITABBLE_H
