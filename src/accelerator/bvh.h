//
// Created by juaquin on 5/12/24.
//

#ifndef RAYTRACING_WEEKEND_BVH_H2
#define RAYTRACING_WEEKEND_BVH_H2


#include "embree4/rtcore.h"
#include <array>

using namespace std;

using Scalar = float;


/// Adapter for BVH library
struct bvhTree {
    RTCDevice device;
    RTCScene scene;
    bvhTree() {
        device = rtcNewDevice(nullptr);
        scene = rtcNewScene(device);
    }

    unsigned int add_mesh(const vector<unsigned int>& index, const vector<float>& vertices) const {
        auto geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);


//        rtcSetNewGeometryBuffer(geom, RTCBufferType::RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,sizeof(float)*3,
//                                vertices.size()/3);
//        auto* vertex_buffer = (float*) rtcGetGeometryBufferData(geom, RTCBufferType::RTC_BUFFER_TYPE_VERTEX, 0);
//        std::copy(vertices.begin(), vertices.end(), vertex_buffer);
        rtcSetSharedGeometryBuffer(geom, RTCBufferType::RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, vertices.data(),
                                   0, sizeof(float)*3, vertices.size()/3);
//        rtcSetSharedGeometryBuffer(geom, RTCBufferType::RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, index.data(), 0, sizeof
//        (unsigned int)*3, index.size()/3);
        rtcSetNewGeometryBuffer(geom, RTCBufferType::RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,sizeof(unsigned int)
        *3, index.size()/3);
        auto* index_buffer = (unsigned int*) rtcGetGeometryBufferData(geom, RTCBufferType::RTC_BUFFER_TYPE_INDEX, 0);
        std::copy(index.begin(), index.end(), index_buffer);

        rtcCommitGeometry(geom);
        unsigned int id = rtcAttachGeometry(scene, geom);
        rtcReleaseGeometry(geom);
        return id;
    }

    unsigned int add_sphere(const array<float, 4>& sphere) const {
        auto geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_SPHERE_POINT);

        rtcSetNewGeometryBuffer(geom, RTCBufferType::RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(float)*4, 1);
        float* t = (float*)rtcGetGeometryBufferData(geom, RTCBufferType::RTC_BUFFER_TYPE_VERTEX, 0);
        memcpy(t, sphere.data(), sizeof(float)*4);

        rtcCommitGeometry(geom);
        unsigned int id = rtcAttachGeometry(scene, geom);
        rtcReleaseGeometry(geom);
        return id;
    }

    void build() const {
        rtcCommitScene(scene);
    }
    void hit(RTCRayHit &ray, Scalar& u, Scalar& v) {
        rtcIntersect1(scene, &ray);
        u = ray.hit.u;
        v = ray.hit.v;
    }

    ~bvhTree() {
        rtcReleaseScene(scene);
        rtcReleaseDevice(device);
    }
};

#endif //RAYTRACING_WEEKEND_BVH_H2
