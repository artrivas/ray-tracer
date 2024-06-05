//
// Created by USER on 4/14/2024.
//

#ifndef RAYTRACING_WEEKEND_COLOR_H
#define RAYTRACING_WEEKEND_COLOR_H

#include "../rtweekend.h"
#include "../interval/interval.h"

using color = vec3;

inline float linear_to_gamma(float linear_component) {
    if (linear_component > 0) {
        return sqrt(linear_component);
    }
    return 0;
}

void write_color(std::ostream& os, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0, 1] component values to the byte range [0, 255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));
    // Write pixel color
    os << rbyte << ' ' << gbyte << ' '<< bbyte << '\n';
}

void write_color(unsigned char* image_buffer, const unsigned int& x, const unsigned int& y, const unsigned int& width,
                 const color&
pixel_color, const int& samples_per_pixel)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples
    // Gamma-correct for gamma=2.0
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    unsigned int index = (y * width + x) * 4;
    static const interval intensity(0.000, 0.999);

    image_buffer[index + 0] = static_cast<unsigned char>(256 * intensity.clamp(r));
    image_buffer[index + 1] = static_cast<unsigned char>(256 * intensity.clamp(g));
    image_buffer[index + 2] = static_cast<unsigned char>(256 * intensity.clamp(b));
    image_buffer[index + 3] = static_cast<unsigned char>(256 * intensity.clamp(1));
}

#endif //RAYTRACING_WEEKEND_COLOR_H
