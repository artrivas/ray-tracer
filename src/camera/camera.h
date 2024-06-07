//
// Created by nick_cq on 06/05/24.
//

#ifndef RAYTRACING_WEEKEND_CAMERA_H
#define RAYTRACING_WEEKEND_CAMERA_H

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../rtweekend.h"

#include "../hittable/hitabble.h"
#include "../material/material.h"
#include "raylib.h"

#include "../ext/stb/stb_image_write.h"
#include "tbb/tbb.h"

#include <chrono>
#include <fstream>
#include <vector>

class camera {
public:
    float aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;  // Maximum number of ray bounces into scene
    color  background;               // Scene background color

    float vfov = 90;  // Vertical view angle (field of view)
    point3 lookfrom = point3(0,0,0);   // Point camera is looking from
    point3 lookat   = point3(0,0,-1);  // Point camera is looking at
    vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

    float defocus_angle = 0;  // Variation angle of rays through each pixel
    float focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void show(const hittable& world) {
        initialize();
        // Render
        unsigned char* image = new unsigned char[image_width * image_height * 4];

        InitWindow(image_width, image_height, "Viewer");
        while (!WindowShouldClose()) {
            initialize();
            _render(world, image);
            if (IsKeyDown(KEY_RIGHT)) { lookfrom += {1, 0, 0};}
            if (IsKeyDown(KEY_LEFT)) lookfrom += {-1, 0, 0};
            if (IsKeyDown(KEY_UP)) lookfrom += {0, 0, 1};
            if (IsKeyDown(KEY_DOWN)) lookfrom += {0, 0, -1};

            BeginDrawing();
            ClearBackground(RAYWHITE);
            for (int y = 0; y < image_height; y++) {
                for (int x = 0; x < image_width; x++) {
                    auto idx  = (y*image_width + x)*4;
                    DrawPixel(x, y, Color(image[idx], image[idx+1], image[idx+2], image[idx+3]));
                }
            }
            EndDrawing();
        }
        CloseWindow();
        delete[] image;
    }
    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;
    void _render(const hittable& world, unsigned char*& image) {
        auto t0 = Time::now();

        auto render_scanline = [this, &world, image](const
                tbb::blocked_range<int>&
                range) {
            for (int j = range.begin(); j < range.end(); ++j) {
                for (int i = 0; i < image_width; ++i) {
                    color pixel_color(0, 0, 0);

                    for (int sample = 0; sample < samples_per_pixel; sample++) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    write_color(image, i, j, image_width, pixel_color, samples_per_pixel);
                }
            }
        };

        tbb::parallel_for(tbb::blocked_range<int>(0, image_height), render_scanline);
        auto t1 = Time::now();
        fsec fs = t1 - t0;
        std::cout << fs.count() << "s\n";
    }

    void render(const hittable& world) {
        initialize();

        // Render
        unsigned char* image = new unsigned char[image_width * image_height * 4];

        _render(world, image);

        std::clog<<"\rDone.        \n";

        stbi_write_png("output.png", image_width, image_height, 4, image, image_width*4);
        delete[] image;
    }

    void render_montecarlo(hittable &world)
    {
        initialize();

        unsigned char* image = new unsigned char[image_width * image_height * 4];

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++)
            {
                color pixel_color(0, 0, 0);
                for (int s_j = 0; s_j < sqrt_spp; s_j++)
                {
                    for (int s_i = 0; s_i < sqrt_spp; s_i++)
                    {
                        ray r = get_ray_montecarlo(i, j, s_i, s_j);
                        pixel_color += ray_color_montecarlo(r, max_depth, world);
                    }
                }
                write_color(image, i, j, image_width, pixel_color, samples_per_pixel);
            }
        }

        stbi_write_png("output_montecarlo.png", image_width, image_height, 4, image, image_width*4);
        delete[] image;
    }

private:
    // file output
    std::ofstream file;

    int    image_height;   // Rendered image height
    float pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;              // Camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius
    int sqrt_spp;               // Square root of number of samples per pixel
    float recip_sqrt_spp;      // 1 / sqrt_spp

    void initialize() {
        // Calculate the image height, and ensure that it's at least 1.
        image_height = int( float(image_width) / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        sqrt_spp = int(sqrt(samples_per_pixel));
        pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
        recip_sqrt_spp = 1.0 / sqrt_spp;
        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);

        // Viewport width < 1 ok since they are in a continuous space
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (float(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;   // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        auto ray_time = random_float(); // generar un tiempo aleatorio entre [0,1)

        return ray(ray_origin, ray_direction, ray_time);
    }

    ray get_ray_montecarlo(int i, int j, int s_i, int s_j) const
    {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j for stratified sample square s_i, s_j.

        auto offset = sample_square_stratified(s_i, s_j);
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_float();

        return ray(ray_origin, ray_direction, ray_time);
    }
    vec3 sample_square_stratified(int s_i, int s_j) const {
        // Returns the vector to a random point in the square sub-pixel specified by grid
        // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

        auto px = ((s_i + random_float()) * recip_sqrt_spp) - 0.5;
        auto py = ((s_j + random_float()) * recip_sqrt_spp) - 0.5;

        return vec3(px, py, 0);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_float() - 0.5, random_float() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    [[nodiscard]] color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we pass ray bounce limit, no more light gathered.
        if (depth <= 0) {
            return {0,0,0};
        }

        hit_record rec;

        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);

        return color_from_emission + color_from_scatter;
    }

    color ray_color_montecarlo(const ray &r, int depth, hittable &world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;

        // If the ray hits nothing, return the background color.
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        float scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
        float pdf = scattering_pdf;

        color color_from_scatter =
            (attenuation * scattering_pdf * ray_color_montecarlo(scattered, depth - 1, world)) / pdf;

        return color_from_emission + color_from_scatter;
    }
};

#endif // RAYTRACING_WEEKEND_CAMERA_H
