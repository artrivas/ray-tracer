//
// Created by USER on 4/14/2024.
//

#ifndef RAYTRACING_WEEKEND_RAY_H
#define RAYTRACING_WEEKEND_RAY_H

#include "../rtweekend.h"

class ray {
public:
    ray() = default;

    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction), tm(0){} //El tiempo que se demora la luz para llegar a un objeto foto-sensible

    ray(const point3& origin, const vec3& direction, double time) : orig(origin), dir(direction), tm(time) {}

    [[nodiscard]] const point3& origin() const { return orig; }
    [[nodiscard]] const vec3& direction() const {return dir; }

    double time() const { return tm;}

    [[nodiscard]] point3 at(double t) const {
        return orig + t*dir;
    }

private:
    point3 orig;
    vec3 dir;
    double tm;
};

#endif //RAYTRACING_WEEKEND_RAY_H
