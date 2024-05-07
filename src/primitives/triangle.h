//
// Created by juaquin on 5/7/24.
//

#ifndef RAYTRACING_WEEKEND_TRIANGLE_H
#define RAYTRACING_WEEKEND_TRIANGLE_H

#include "../hittable/hitabble.h"
#include "../rtweekend.h"
#include <initializer_list>

class triangle : public hittable {
private:
    point3 vertexes[3];
public:
    triangle(const point3 &vertex1, const point3 &vertex2, const point3 &vertex3) {
        this->vertexes[0] = vertex1;
        this->vertexes[1] = vertex2;
        this->vertexes[2] = vertex3;
    };

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        vec3 edge1 = this->vertexes[1] - this->vertexes[0];
        vec3 edge2 = this->vertexes[2] - this->vertexes[0];
        vec3 ray_cross = cross(r.direction(), edge2);

        float det = dot(edge1, ray_cross);

        if (det > -e && det < e) return false;

        float inv_det = 1. / det;
        vec3 s = r.origin() - vertexes[0];
        float u = inv_det * dot(s, ray_cross);

        if (u < 0 || u > 1) return false;

        vec3 s_cross_e1 = cross(s, edge1);
        float v = inv_det * dot(r.direction(), s_cross_e1);

        if (v < 0 || u + v > 1) return false;

        float t = inv_det * dot(edge2, s_cross_e1);

        if (t > e and ray_t.contains(t)) {
            rec.t = t;
            rec.p = r.at(t);
            return true;
        } else return false;
    }
};

#endif //RAYTRACING_WEEKEND_TRIANGLE_H
