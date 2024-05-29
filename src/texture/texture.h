//
// Created by nick_cq on 16/05/24.
//

#ifndef RAYTRACING_WEEKEND_TEXTURE_H
#define RAYTRACING_WEEKEND_TEXTURE_H

#include "../perlin/perlin.h"
#include "../rtweekend.h"

class texture {

public:
    virtual ~texture() = default;

    virtual color value(float u, float v, const point3& p) const = 0;
};

class solid_color : public texture {
private:
    color albedo;
public:
    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(float red, float green, float blue) : solid_color(color(red,green,blue)) {}

    color value(float u, float v, const point3& p) const override {
        return albedo;
    }

};

class checker_texture : public texture {
public:
    checker_texture(float scale, shared_ptr<texture> even, shared_ptr<texture> odd)
            : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(float scale, const color& c1, const color& c2)
            : inv_scale(1.0 / scale),
              even(make_shared<solid_color>(c1)),
              odd(make_shared<solid_color>(c2))
    {}

    color value(float u, float v, const point3& p) const override {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    float inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

class noise_texture : public texture {
private:
    float scale;
    perlin noise;
public:
    noise_texture() {}

    noise_texture(float scale) : scale(scale) {}

    color value(float u, float v, const point3& p) const override {
        return color(.5, .5, .5) * (1 + sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }

};

#endif //RAYTRACING_WEEKEND_TEXTURE_H
