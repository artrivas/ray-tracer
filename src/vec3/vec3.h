//
// Created by USER on 4/14/2024.
//

#ifndef RAYTRACING_WEEKEND_VEC3_H
#define RAYTRACING_WEEKEND_VEC3_H

#include "../rtweekend.h"

class vec3 {
public:
    double e[3];
    vec3(): e{0,0,0} {};
    vec3(double e0, double e1, double e2): e{e0, e1, e2} {};

    [[nodiscard]] double x() const { return e[0]; }
    [[nodiscard]] double y() const { return e[1]; }
    [[nodiscard]] double z() const { return e[2]; }

    vec3 operator-() const { return {-e[0], -e[1], -e[2]}; };
    double operator[](int i) const { return e[i]; };
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    [[nodiscard]] double length() const {
        return sqrt(length_squared());
    }

    [[nodiscard]] double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
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

inline vec3 operator*(double t, const vec3& v) {
    return {t * v.e[0], t * v.e[1], t * v.e[2] };
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
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

#endif //RAYTRACING_WEEKEND_VEC3_H
