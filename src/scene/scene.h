//
// Created by juaquin on 5/22/24.
//

#ifndef RAYTRACING_WEEKEND_SCENE_H
#define RAYTRACING_WEEKEND_SCENE_H

#include "../rtweekend.h"
#include "../hittable/hitabble.h"
#include "../accelerator/bvh.h"
#include "../primitives/mesh.h"

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
    bool hit(const ray& r, interval ray_t, hit_record& rec) override {
        RTCRayHit ray{};
        ray.ray.org_x = float(r.origin().x());
        ray.ray.org_y = float(r.origin().y());
        ray.ray.org_z = float(r.origin().z());

        ray.ray.dir_x = float(r.direction().x());
        ray.ray.dir_y = float(r.direction().y());
        ray.ray.dir_z = float(r.direction().z());
        ray.ray.tnear = 0;
        ray.ray.tfar = std::numeric_limits<float>::infinity();
        ray.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        ray.ray.mask = 0x1;

        float u, v;
        this->bvh->hit(ray, u, v);
        if (ray.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            auto t = objs.at(ray.hit.geomID);
            t->set_material(rec, ray.hit.primID, u, v);
            rec.t = ray.ray.tfar;
            rec.p = r.at(rec.t);
            rec.set_face_normal(r, unit_vector(vec3(ray.hit.Ng_x, ray.hit.Ng_y, ray.hit.Ng_z)));
            return true;
        }
        return false;
    }

    void build() {
        bvh->build();
    }
};

#endif //RAYTRACING_WEEKEND_SCENE_H
