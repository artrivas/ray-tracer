//
// Created by USER on 4/14/2024.
//

#ifndef RAYTRACING_WEEKEND_COLOR_H
#define RAYTRACING_WEEKEND_COLOR_H

#include "../rtweekend.h"

using color = vec3;

void write_color(std::ostream& os, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Translate the [0, 1] component values to the byte range [0, 255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));
    // Write pixel color
    os << rbyte << ' ' << gbyte << ' '<< bbyte << '\n';
}

#endif //RAYTRACING_WEEKEND_COLOR_H
