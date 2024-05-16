//
// Created by juaquin on 5/12/24.
//

#ifndef RAYTRACING_WEEKEND_BVH_H2
#define RAYTRACING_WEEKEND_BVH_H2

#include "bvh/v2/bvh.h"
#include "bvh/v2/executor.h"
#include "bvh/v2/thread_pool.h"
#include "bvh/v2/bbox.h"
#include "bvh/v2/tri.h"
#include "bvh/v2/default_builder.h"
#include "bvh/v2/stack.h"
#include "../vec3/vec3.h"
#include "../hittable/hitabble.h"


using namespace std;

using Scalar = float;
using Vec3 = bvh::v2::Vec<Scalar, 3>;
using BBox = bvh::v2::BBox<Scalar, 3>;
using Tri = bvh::v2::Tri<Scalar, 3>;
using Node = bvh::v2::Node<Scalar, 3>;
using Bvh = bvh::v2::Bvh<Node>;
using Ray = bvh::v2::Ray<Scalar, 3>;

using PrecomputedTri = bvh::v2::PrecomputedTri<Scalar>;

namespace convert {
    Vec3 to(const vec3 &v) {
        return {static_cast<float>(v.x()), static_cast<float>(v.y()), v.z()};
    }

    vec3 from(const Vec3 &v) {
        return {v.values[0], v.values[1], v.values[2]};
    }
}


/// Adapter for BVH library
struct bvhTree {
    shared_ptr<Bvh> bvh;
    vector<PrecomputedTri> precomputed;

    /// lm function is used for insert some in the same order of bvh tri precomputed
    bvhTree(const std::vector<Tri>& tris) {

        bvh::v2::ThreadPool thread_pool;
        bvh::v2::ParallelExecutor executor(thread_pool);

        std::vector<BBox> bboxes(tris.size());
        std::vector<Vec3> centers(tris.size());

        executor.for_each(0, tris.size(), [&](size_t begin, size_t end) {
            for (size_t i = begin; i < end; ++i) {
                bboxes[i] = tris[i].get_bbox();
                centers[i] = tris[i].get_center();
            }
        });

        typename bvh::v2::DefaultBuilder<Node>::Config config;
        config.quality = bvh::v2::DefaultBuilder<Node>::Quality::High;

        this->bvh = make_shared<Bvh>(bvh::v2::DefaultBuilder<Node>::build(thread_pool, bboxes, centers, config));

        this->precomputed.resize(tris.size());
        executor.for_each(0, tris.size(), [&](size_t begin, size_t end) {
            for (size_t i = begin; i < end; ++i) {
                auto j = this->bvh->prim_ids[i];
                this->precomputed[i] = tris[j];
            }
        });
    }

    size_t hit(Ray &ray, hit_record &rec, Scalar& u, Scalar& v) {
        static constexpr size_t invalid_id = std::numeric_limits<size_t>::max();
        static constexpr size_t stack_size = 64;
        static constexpr bool use_robust_traversal = false;

        auto prim_id = invalid_id;

        bvh::v2::SmallStack<Bvh::Index, stack_size> stack;
        bvh->intersect<false, use_robust_traversal>(ray, bvh->get_root().index, stack,
                                                    [&](size_t begin, size_t end) {
                                                        for (size_t i = begin; i < end; ++i) {
                                                            size_t j = i;
                                                            if (auto hit = precomputed[j].intersect(ray)) {
                                                                prim_id = i;
                                                                std::tie(u, v) = *hit;
                                                            }
                                                        }
                                                        return prim_id != invalid_id;
                                                    });
        return prim_id;

    }
};

#endif //RAYTRACING_WEEKEND_BVH_H2
