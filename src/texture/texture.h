//
// Created by nick_cq on 16/05/24.
//

#ifndef RAYTRACING_WEEKEND_TEXTURE_H
#define RAYTRACING_WEEKEND_TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION

#include "../perlin/perlin.h"
#include "../rtweekend.h"
#include "../ext/stb/stb_image.h"
#include "nlohmann/json.hpp"
class texture {

public:
    virtual ~texture() = default;

    virtual color value(const float& u, const float& v, const point3& p) const = 0;
};
class solid_color : public texture {
private:
    color albedo;
public:
    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(float red, float green, float blue) : solid_color(color(red,green,blue)) {}

    color value(const float& u, const float& v, const point3& p) const override {
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

    color value(const float& u, const float& v, const point3& p) const override {
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

    color value(const float& u, const float& v, const point3& p) const override {
        return color(.5, .5, .5) * (1 + sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }
};

class texture_image: public texture {
    unsigned char *imageData;
    int width{}, height{}, channels{};
public:
    texture_image(const std::string& path) {
        this->imageData = stbi_load(path.c_str(), &width, &height, &channels, STBI_ORDER_RGB);
        if (!imageData) {
            std::cerr << "Error: Can't load the image " << path << std::endl;
        }
    }

    [[nodiscard]] static float gamma_to_linear(float value, float gamma = 2.2) {
        return pow(value, 1/gamma);
    }

    color value(const float& u, const float& v, const point3& p) const override {
        color c;
        const int _u = static_cast<int>(u*width);
        const int _v = static_cast<int>(height*(1 - v));

        const int index = (_v*this->width + _u)*this->channels;
        const float gamma = imageData[index + 3] / 255.;
        c.e[0] = gamma_to_linear(imageData[index] / 255., gamma);
        c.e[1] = gamma_to_linear(imageData[index + 1] / 255., gamma);
        c.e[2] = gamma_to_linear(imageData[index + 2] / 255., gamma);
        return c;
    }

    ~texture_image() {
        stbi_image_free(imageData);
    }
};

void from_json(const json& camera, shared_ptr<texture>& t) {
    if (camera.contains("color")) {
        t = make_shared<solid_color>(camera["color"].get<point3>());
    }
    else if(camera.contains("checker")) {
        auto scale = camera["checker"]["scale"].get<float>();
        auto even = camera["checker"]["even"].get<point3>();
        auto odd = camera["checker"]["odd"].get<point3>();
        t = make_shared<checker_texture>(scale, make_shared<solid_color>(even), make_shared<solid_color>(odd));
    }
    else if (camera.contains("noise")) {
        auto scale = camera["noise"]["scale"].get<float>();
        t = make_shared<noise_texture>(scale);
    } else if (camera.contains("image")) {
        auto path = camera["image"]["path"].get<std::string>();
        t = make_shared<texture_image>(path);
    }
}

#endif //RAYTRACING_WEEKEND_TEXTURE_H
