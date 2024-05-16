//
// Created by nick_cq on 06/05/24.
//

#ifndef RAYTRACING_WEEKEND_INTERVAL_H
#define RAYTRACING_WEEKEND_INTERVAL_H

#include "../rtweekend.h"

class interval {
public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    interval(double min, double max) : min(min), max(max) {}

    double size() const {
        return max-min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }
    
    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    [[nodiscard]] interval expand(double delta) const {
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
