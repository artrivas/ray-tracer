//
// Created by juaquin on 5/27/24.
//

#ifndef RAYTRACING_WEEKEND_PRIMITIVE_H
#define RAYTRACING_WEEKEND_PRIMITIVE_H

#include "../hittable/hitabble.h"
#include <array>

class primitive {
public:
    virtual void set_material(hit_record& rec, const unsigned int& primID, const float& u, const float& v) = 0;
};

#endif //RAYTRACING_WEEKEND_PRIMITIVE_H
