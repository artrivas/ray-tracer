//
// Created by USER on 5/4/2024.
//

#ifndef RAYTRACING_WEEKEND_HITTABLE_LIST_H
#define RAYTRACING_WEEKEND_HITTABLE_LIST_H

#include "hitabble.h"
#include "../rtweekend.h"

#include <vector>


class hittable_list: public hittable {
public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() = default;
    explicit hittable_list(const shared_ptr<hittable>& object) {add(object);}

    void clear() { objects.clear(); }

    void add(const shared_ptr<hittable>& object) {
        objects.push_back(object);
    }

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_tmax;

        for (const auto& object : objects) {
            if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }
};

#endif //RAYTRACING_WEEKEND_HITTABLE_LIST_H
