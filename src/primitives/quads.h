//
// Created by nick_cq on 17/05/24.
//

#ifndef RAYTRACING_WEEKEND_QUADS_H
#define RAYTRACING_WEEKEND_QUADS_H

#include "../rtweekend.h"
#include "primitive.h"

class quad : public primitive {
private:
    shared_ptr<material> mat;
    point3 origin;
    point3 u, v;
public:
    quad(const point3& Q, const vec3& u, const vec3& v, const shared_ptr<material>& mat) : mat(mat) {
        this->origin = Q;
        this->u = u;
        this->v = v;
    }

    void set_material(hit_record& rec, const unsigned int& primID, const float& u, const float& v) override  {
        rec.mat = this->mat;
    }

    std::array<std::array<float, 3>, 4> get_quad() {
        auto p1 = origin + u;
        auto p2 = origin + v;
        auto p3 = origin + u + v;

        std::array<std::array<float, 3>, 4> ans{};
        ans[0] = {static_cast<float>(origin.x()), static_cast<float>(origin.y()), static_cast<float>(origin.z())};
        ans[1] = {static_cast<float>(p1.x()), static_cast<float>(p1.y()), static_cast<float>(p1.z())};
        ans[2] = {static_cast<float>(p3.x()), static_cast<float>(p3.y()), static_cast<float>(p3.z())};
        ans[3] = {static_cast<float>(p2.x()), static_cast<float>(p2.y()), static_cast<float>(p2.z())};

        return ans;
    }
};

#endif //RAYTRACING_WEEKEND_QUADS_H
