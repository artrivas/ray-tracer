#include "rtweekend.h"

#include "camera/camera.h"
#include "hittable/hittable_list.h"
#include "primitives/sphere.h"
#include "primitives/triangle.h"

int main() {
    // World
    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1),0.5));
    world.add(make_shared<triangle>(point3(-0.5, -0.5, -0.6), point3(0.5, -0.5, -0.6), point3(0, 1, -0.6)));

    //   world.add(make_shared<triangle>(point3(-1, 0, -0.5), point3(1, 0, -0.5), point3(0, 2, -2)));
    //   world.add(make_shared<triangle>(point3(-1, 0, -1), point3(1, 0, -1), point3(0, 2, -1)));

    world.add(make_shared<sphere>(point3(0,-100.5,-1),100));
    // Camera
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    
    cam.render(world);
    return 0;
}
