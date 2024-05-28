//
// Created by nick_cq on 08/05/24.
//

#ifndef RAYTRACING_WEEKEND_MATERIAL_H
#define RAYTRACING_WEEKEND_MATERIAL_H

#include "../rtweekend.h"
#include "../texture/texture.h"

class hit_record;

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const
    {
        return false;
    }
    virtual double scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered)
        const
    {
        return 0;
    }

    [[nodiscard]] virtual color emitted(double u, double v, const point3 &p) const
    {
        return {0, 0, 0};
    }
};

class lambertian : public material
{
private:
    color albedo;
    shared_ptr<texture> tex;

public:
    lambertian(const color &albedo) : tex(make_shared<solid_color>(albedo)){};
    lambertian(shared_ptr<texture> tex) : tex(tex){};

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero())
        {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction, r_in.time());

        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

    double scattering_pdf(const ray &r_in, const hit_record &rec, const ray &scattered) const
    {
        auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi;
    }
};

class metal : public material
{
private:
    color albedo;
    double fuzz;

public:
    metal(const color &albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected, r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

class dielectric : public material
{
private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index)
    {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        {
            direction = reflect(unit_direction, rec.normal);
        }
        else
        {
            direction = refract(unit_direction, rec.normal, ri);
        }

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }
};

class diffuse_light : public material
{
public:
    explicit diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
    explicit diffuse_light(const color &emit) : tex(make_shared<solid_color>(emit)) {}

    [[nodiscard]] color emitted(double u, double v, const point3 &p) const override
    {
        return tex->value(u, v, p);
    }

private:
    shared_ptr<texture> tex;
};

#endif // RAYTRACING_WEEKEND_MATERIAL_H
