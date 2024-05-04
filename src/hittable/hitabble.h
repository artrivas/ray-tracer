//
// Created by USER on 5/2/2024.
//

#ifndef RAYTRACING_WEEKEND_HITABBLE_H
#define RAYTRACING_WEEKEND_HITABBLE_H

#include "../rtweekend.h"

class hit_record {
public:
    point3  p;
    vec3 normal;
    double t;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: The parameter outward_normal is assumed to have unit lenght.

        // ray is inside the sphere if dot product > 0
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif //RAYTRACING_WEEKEND_HITABBLE_H
