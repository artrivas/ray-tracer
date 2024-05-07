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

// C++ Std usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
const float e = std::numeric_limits<float>::epsilon();

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}


inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

// Common Headers
#include "vec3/vec3.h"
#include "color/color.h"
#include "interval/interval.h"
#include "ray/ray.h"


#endif //RAYTRACING_WEEKEND_RTWEEKEND_H
