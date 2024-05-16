//
// Created by USER on 5/2/2024.
//

#ifndef RAYTRACING_WEEKEND_SPHERE_H
#define RAYTRACING_WEEKEND_SPHERE_H

#include "../hittable/hitabble.h"
#include "../rtweekend.h"

class sphere: public hittable {
private:
    point3 center1;
    point3 center;
    double radius;
    shared_ptr<material> mat;

    bool is_moving;
    vec3 center_vec;

    point3 sphere_center(double time) const {
        // Linearly interpolate from center1 to center2 according to time, where t=0 yields
        // center1, and t=1 yields center2.
        return center1 + time*center_vec;
    }

public:

    // Stationary Sphere
    sphere(const point3& center, double radius, shared_ptr<material> mat)
      : center1(center), radius(fmax(0,radius)), mat(mat), is_moving(false) {}

    // Moving Sphere
    sphere(const point3& center1, const point3& center2, double radius,
           shared_ptr<material> mat)
      : center1(center1), radius(fmax(0,radius)), mat(mat), is_moving(true)
    {
        center_vec = center2 - center1;
    }

    //sphere(const point3& center, double radius, shared_ptr<material> mat): center(center), radius(fmax(0,radius)), mat(mat) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        point3 center = is_moving ? sphere_center(r.time()) : center1;
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0) {
            return false;
        }

        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }
};

#endif //RAYTRACING_WEEKEND_SPHERE_H
