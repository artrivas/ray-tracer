//
// Created by USER on 5/4/2024.
//

#ifndef RAYTRACING_WEEKEND_RTWEEKEND_H
#define RAYTRACING_WEEKEND_RTWEEKEND_H

#include <cmath>
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

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// Common Headers
#include "vec3/vec3.h"
#include "color/color.h"
#include "ray/ray.h"


#endif //RAYTRACING_WEEKEND_RTWEEKEND_H
