//
// Created by USER on 5/4/2024.
//

#ifndef RAYTRACING_WEEKEND_RTWEEKEND_H
#define RAYTRACING_WEEKEND_RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

// C++ Std usings

using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;
const float e = std::numeric_limits<float>::epsilon();

// Utility Functions

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}


inline float random_float() {
    // Returns a random real in [0,1).
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<float> distribution(0, 1);
    return distribution(generator);
}

inline float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_float();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return int(random_float(min, max+1));
}

// Common Headers
#include "vec3/vec3.h"
#include "color/color.h"
#include "interval/interval.h"
#include "ray/ray.h"


#endif //RAYTRACING_WEEKEND_RTWEEKEND_H
