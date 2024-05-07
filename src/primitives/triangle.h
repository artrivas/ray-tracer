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
        return false;
    }
};

#endif //RAYTRACING_WEEKEND_TRIANGLE_H
