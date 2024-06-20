#include "rtweekend.h"

#include "camera/camera.h"
#include "hittable/hittable_list.h"
#include "primitives/quads.h"
#include "primitives/mesh.h"
#include "texture/texture.h"
#include "scene/scene.h"

#include "ext/json/json.hpp"

void custom() {
    scene world;
    auto t = mesh("../samples/bugatti/bugatti.obj");

//    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
//    auto material_left   = make_shared<dielectric>(float(1.50) );
//    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
//    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
//
//    world.add_sphere(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
//    world.add_sphere(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
//    world.add_sphere(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
//    world.add_sphere(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    world.add_obj(make_shared<mesh>(t));

//    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
//    world.add_sphere(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));
    world.build();

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 10;
    cam.max_depth         = 3;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1920;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 3;
    cam.background        = color(1, 1, 1);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,30);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

//    cam.show(world);
//    cam.render(world);
    cam.render_montecarlo(world);
}

void checkered_spheres() {
    scene world;
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    world.add_sphere(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    world.add_sphere(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));
    world.build();
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 3;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void perlin() {
    scene world;

    auto pertext = make_shared<noise_texture>(4);
    world.add_sphere(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add_sphere(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));
    world.build();

    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render_montecarlo(world);
}

//void quads() {
//    hittable_list world;
//
//    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
//    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
//    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
//    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
//    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));
//
//    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
//    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
//    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
//    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
//    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));
//
//    camera cam;
//
//    cam.aspect_ratio      = 1.0;
//    cam.image_width       = 400;
//    cam.samples_per_pixel = 100;
//    cam.max_depth         = 50;
//    cam.background        = color(0.70, 0.80, 1.00);
//
//    cam.vfov     = 80;
//    cam.lookfrom = point3(0,0,9);
//    cam.lookat   = point3(0,0,0);
//    cam.vup      = vec3(0,1,0);
//
//    cam.defocus_angle = 0;
//
//    cam.render(world);
//}

void simple_light() {
    scene world;

    auto pertext = make_shared<noise_texture>(4);
    world.add_sphere(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add_sphere(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add_sphere(make_shared<sphere>(point3(0,7,0), 2, difflight));
    world.add_quad(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));
    world.build();
    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 20;
    cam.lookfrom = point3(26,3,6);
    cam.lookat   = point3(0,2,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

//void cornell_box() {
//    hittable_list world;
//
//    auto red   = make_shared<lambertian>(color(.65, .05, .05));
//    auto white = make_shared<lambertian>(color(.73, .73, .73));
//    auto green = make_shared<lambertian>(color(.12, .45, .15));
//    auto light = make_shared<diffuse_light>(color(15, 15, 15));
//
//    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
//    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
//    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
//    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
//    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
//    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));
//
//    camera cam;
//
//    cam.aspect_ratio      = 1.0;
//    cam.image_width       = 600;
//    cam.samples_per_pixel = 200;
//    cam.max_depth         = 50;
//    cam.background        = color(0,0,0);
//
//    cam.vfov     = 40;
//    cam.lookfrom = point3(278, 278, -800);
//    cam.lookat   = point3(278, 278, 0);
//    cam.vup      = vec3(0,1,0);
//
//    cam.defocus_angle = 0;
//
//    cam.render(world);
//}

void custom2() {
    scene world;
    auto t = mesh("../samples/cat/12221_Cat_v1_l3.obj"); // , {0, 22, -30}
    t.set_origin({0, 22, -30});
    t.rescale(0.2);

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    auto checker = make_shared<checker_texture>(0.32, color(.5, .0, .1), color(.9, .9, .9));

    world.add_sphere(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));
    world.add_obj(make_shared<mesh>(t));
    world.add_sphere(make_shared<sphere>(point3(0, 30, 0), 2, difflight));
    world.build();


    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 1;
    cam.max_depth         = 3;
    cam.background        = color(1.0, 1.0, 1.0);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

//    cam.render(world);
    cam.show(world);
}

void custom3() {
    scene world;
    auto t = mesh("../samples/Buddha1.obj", make_shared<metal>(color(1.0,0.5,0.0), 0.0)); // , {0, 22, -30}

    world.add_sphere(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(color(1.0,0.0,0.0))));

    world.add_obj(make_shared<mesh>(t));
    world.build();

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 1;
    cam.max_depth         = 3;
    cam.background        = color(0.7, 0.7, 0.7);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.show(world);
//    cam.render(world);
//    cam.render_montecarlo(world);
}

void custom4() {
    scene world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add_sphere(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add_sphere(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add_sphere(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add_sphere(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    world.build();

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1920;
    cam.samples_per_pixel = 100;

    cam.max_depth         = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.render(world);
//    cam.render_montecarlo(world);
}

void dielectric_scene() {
    scene world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add_sphere(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add_sphere(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add_sphere(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add_sphere(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add_sphere(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    world.build();

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;

    cam.max_depth         = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.render(world);
//    cam.render_montecarlo(world);
}

void background_scene() {
    scene world;

    auto text_sphere = make_shared<texture_image>("../images/golden_bay_2k.hdr");
    auto pertext = make_shared<noise_texture>(4);
    auto checker = make_shared<checker_texture>(0.32, color(.0, .0, .1), color(.9, .9, .9));

    world.add_sphere(make_shared<sphere>(point3( 0.0, 0.0, 0.0), 100.0, make_shared<diffuse_light>(text_sphere)));
    world.build();

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 1;

    cam.max_depth         = 3;
    cam.background        = color(1.00, 1.00, 1.00);

    cam.lookfrom = point3(0,0,20);
    cam.lookat   = point3(0,0,0);

//    cam.show(world);
   cam.render(world);
//    cam.render_montecarlo(world);
}

void parse(const string& filename) {
    std::ifstream f(filename);
    auto data = json::parse(f);


    camera cam = data["camera"].get<camera>();
    vector<shared_ptr<texture>> sc = data["textures"].get<vector<shared_ptr<texture>>>();
    vector<shared_ptr<material>> m;
    for (auto& i : data["materials"]) {
        if (i["type"] == "lambertian") {
            m.push_back(make_shared<lambertian>(sc[i["texture"]]));
        } else if (i["type"] == "metal") {
            m.push_back(make_shared<metal>(sc[i["texture"]], i["fuzz"]));
        } else if (i["type"] == "dielectric") {
            m.push_back(make_shared<dielectric>(i["refraction"]));
        } else if (i["type"] == "diffuse_light") {
            m.push_back(make_shared<diffuse_light>(sc[i["texture"]]));
        }
    }
    scene world;
    for (auto& i : data["objects"]) {
        if (i["type"].get<std::string>() == "sphere") {
            world.add_sphere(make_shared<sphere>(i["center"], i["radius"], m[i["material"]]));
        } else if (i["type"] == "quad") {
            world.add_quad(make_shared<quad>(i["center"], i["u"], i["v"], m[i["material"]]));
        } else if (i["type"] == "mesh") {
            auto t = mesh(i["path"]);
            t.set_origin(i["center"]);
            t.rescale(i["scale"]);
            world.add_obj(make_shared<mesh>(t));
        }
    }
    world.build();
    if (data.find("output") != data.end()) {
        cam.output = data["output"].get<std::string>();
    }

    if (data["mode"]  == 0) {
        cam.show(world);
    } else if (data["mode"]  == 1) {
        cam.render(world);
    } else if (data["mode"]  == 2) {
        cam.render_montecarlo(world);
    }
}

int main(int argc, char** argv) {
    if (argc == 2) {
        std::string filename = argv[1];
        parse(filename);
    }
    else return 1;
}
