//
// Created by nick_cq on 06/05/24.
//

#ifndef RAYTRACING_WEEKEND_INTERVAL_H
#define RAYTRACING_WEEKEND_INTERVAL_H

#include "../rtweekend.h"

class interval {
public:
    float min, max;

    interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    interval(float min, float max) : min(min), max(max) {}

    float size() const {
        return max-min;
    }

    bool contains(float x) const {
        return min <= x && x <= max;
    }

    bool surrounds(float x) const {
        return min < x && x < max;
    }
    
    float clamp(float x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    [[nodiscard]] interval expand(float delta) const {
        auto padding = delta/2;
        return {min - padding, max + padding};
    }

    interval(const interval& a, const interval& b) {
        // Create the interval tightly enclosing the two input intervals.
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif //RAYTRACING_WEEKEND_INTERVAL_H
