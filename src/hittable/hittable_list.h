//
// Created by USER on 5/4/2024.
//

#ifndef RAYTRACING_WEEKEND_HITTABLE_LIST_H
#define RAYTRACING_WEEKEND_HITTABLE_LIST_H

#include "../aabb/aabb.h"
#include "hitabble.h"
#include "../rtweekend.h"

#include <vector>


class hittable_list: public hittable {
private:
    aabb bbox;
public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() = default;
    explicit hittable_list(const shared_ptr<hittable>& object) {add(object);}

    void clear() { objects.clear(); }

    void add(const shared_ptr<hittable>& object) {
        objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }

    aabb bounding_box() const override { return bbox; }
};

#endif //RAYTRACING_WEEKEND_HITTABLE_LIST_H
