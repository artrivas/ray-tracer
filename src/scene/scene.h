//
// Created by juaquin on 5/22/24.
//

#ifndef RAYTRACING_WEEKEND_SCENE_H
#define RAYTRACING_WEEKEND_SCENE_H

#include "../rtweekend.h"
#include "../hittable/hitabble.h"
#include "../accelerator/bvh.h"
#include "../primitives/sphere.h"
#include "../primitives/mesh.h"
#include "../primitives/quads.h"

class scene: public hittable {
public:
    unique_ptr<bvhTree> bvh;
    vector<shared_ptr<primitive>> objs;
    scene() {
        bvh = make_unique<bvhTree>();
    }

    void add_sphere(const shared_ptr<sphere>& obj) {
        auto t = obj->get_sphere();
        auto id = bvh->add_sphere(t);
        cout << id << endl;
        objs.push_back(obj);
    }
    void add_obj(const shared_ptr<mesh>& obj) {
        auto indices = obj->get_indices();
        auto id = bvh->add_mesh(indices, obj->attrib.vertices);
        cout << id << endl;
        objs.push_back(obj);
    }
    void add_quad(const shared_ptr<quad>& obj) {
        array<unsigned int, 4> indices = {0, 1, 2, 3};
        auto vertices = obj->get_quad();
        auto id = bvh->add_quad(indices, vertices);
        cout << id << endl;
        objs.push_back(obj);
    }
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        RTCRayHit ray{};
        ray.ray.org_x = r.origin().x();
        ray.ray.org_y = r.origin().y();
        ray.ray.org_z = r.origin().z();

        ray.ray.dir_x = r.direction().x();
        ray.ray.dir_y = r.direction().y();
        ray.ray.dir_z = r.direction().z();
        ray.ray.tnear = 0;
        ray.ray.tfar = std::numeric_limits<float>::infinity();
        ray.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        ray.ray.mask = 0x1;

        float u, v;
        this->bvh->hit(ray, u, v);
        if (ray.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            auto t = objs.at(ray.hit.geomID);
            rec.t = ray.ray.tfar;
            const auto normal = unit_vector(vec3(ray.hit.Ng_x, ray.hit.Ng_y, ray.hit.Ng_z));
            rec.set_face_normal(r, normal);

            // Displacement improves black acne in image. Prevents origin of scattered ray to start below the surface
            // because of floating point precision when calculating the point p.
            rec.p = r.at(rec.t);
            t->set_material(rec, ray.hit.primID, u, v);
            return true;
        }
        return false;
    }

    void build() {
        bvh->build();
    }
};

#endif //RAYTRACING_WEEKEND_SCENE_H
