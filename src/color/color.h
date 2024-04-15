//
// Created by USER on 4/14/2024.
//

#ifndef RAYTRACING_WEEKEND_COLOR_H
#define RAYTRACING_WEEKEND_COLOR_H

#include "../vec3/vec3.h"

#include <iostream>

using color = vec3;

void write_color(std::ostream& os, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Translate the [0, 1] component values to the byte range [0, 255].
    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    // Write pixel color
    os << rbyte << ' ' << gbyte << ' '<< bbyte << '\n';
}

#endif //RAYTRACING_WEEKEND_COLOR_H
