//
// Created by USER on 4/14/2024.
//

#ifndef RAYTRACING_WEEKEND_VEC3_H
#define RAYTRACING_WEEKEND_VEC3_H

#include "../rtweekend.h"

class vec3 {
public:
    float e[3];
    vec3(): e{0,0,0} {};
    vec3(float e0, float e1, float e2): e{e0, e1, e2} {};
    vec3(const float data[3]) {
        std::copy(data, data + 3, e);
    }

    [[nodiscard]] float x() const { return e[0]; }
    [[nodiscard]] float y() const { return e[1]; }
    [[nodiscard]] float z() const { return e[2]; }

    vec3 operator-() const { return {-e[0], -e[1], -e[2]}; };
    float operator[](int i) const { return e[i]; };
    float& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(float t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(float t) {
        return *this *= 1/t;
    }

    [[nodiscard]] float length() const {
        return sqrt(length_squared());
    }

    [[nodiscard]] float length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    [[nodiscard]] bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    static vec3 random() {
        return {random_float(), random_float(), random_float()};
    }

    static vec3 random(float min, float max) {
        return {random_float(min, max), random_float(min, max), random_float(min, max)};
    }
};

// point3 is just an alias for vec3
using point3 = vec3;

// Vector Utility Functions
inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
    return os<<v.e[0]<<' '<<v.e[1]<<' '<<v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return {u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2] };
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return {u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2] };
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return {u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2] };
}

inline vec3 operator*(float t, const vec3& v) {
    return {t * v.e[0], t * v.e[1], t * v.e[2] };
}

inline vec3 operator*(const vec3& v, float t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, float t) {
    return (1/t) * v;
}

inline float dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return {u.e[1]*v.e[2] - u.e[2]*v.e[1],
            u.e[2]*v.e[0] - u.e[0]*v.e[2],
            u.e[0]*v.e[1] - u.e[1]*v.e[0]
            };
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_float(-1,1), random_float(-1,1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() < 1) {
            return p;
        }
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) { // Same hemisphere as normal
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif //RAYTRACING_WEEKEND_VEC3_H
