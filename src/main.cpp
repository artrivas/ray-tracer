#include "rtweekend.h"

#include "camera/camera.h"
#include "hittable/hittable_list.h"
#include "primitives/sphere.h"
#include "primitives/triangle.h"

int main() {
    // World
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    auto material_triangle = make_shared<metal>(color(1, 0.2, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    //   world.add(make_shared<triangle>(point3(-0.5, -0.5, -0.6), point3(0.5, -0.5, -0.6), point3(0, 1, -0.6)));
    world.add(make_shared<triangle>(point3(-3, 0, -3), point3(3, 0, -3), point3(0, 2, -2),material_triangle));
    //   world.add(make_shared<triangle>(point3(-1, 0, -1), point3(1, 0, -1), point3(0, 2, -1)));

    // Camera
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    
    cam.render(world);
    return 0;
}
