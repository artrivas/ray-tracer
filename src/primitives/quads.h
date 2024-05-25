//
// Created by nick_cq on 17/05/24.
//

#ifndef RAYTRACING_WEEKEND_QUADS_H
#define RAYTRACING_WEEKEND_QUADS_H

#include "../rtweekend.h"

#include "../hittable/hitabble.h"

class quad : public hittable {
private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    shared_ptr<material> mat;
    vec3 normal;
    double D;
public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat) {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n,n);
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        interval unit_interval = interval(0, 1);

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) override {
        auto denom = dot(normal, r.direction());

        if (fabs(denom) < 1e-8)
            return false;

        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t)) {
            return false;
        }


        auto intersection = r.at(t);
        vec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec)) {
            return false;
        }

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }


};

#endif //RAYTRACING_WEEKEND_QUADS_H
