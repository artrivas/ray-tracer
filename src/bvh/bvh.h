//
// Created by nick_cq on 15/05/24.
//

#ifndef RAYTRACING_WEEKEND_BVH_H
#define RAYTRACING_WEEKEND_BVH_H

#include "../rtweekend.h"

#include "../aabb/aabb.h"
#include "../hittable/hitabble.h"
#include "../hittable/hittable_list.h"

class bvh_node : public hittable {
private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;
public:
    explicit bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
        // To be implemented later.
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox.hit(r, ray_t))
            return false;

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override { return bbox; }
};

#endif //RAYTRACING_WEEKEND_BVH_H
