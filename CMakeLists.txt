cmake_minimum_required(VERSION 3.28)
project(raytracing_weekend)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(src/ext/bvh)
add_executable(raytracing_weekend src/main.cpp
        src/vec3/vec3.h
        src/color/color.h
        src/ray/ray.h
        src/hittable/hitabble.h
        src/primitives/sphere.h
        src/hittable/hittable_list.h
        src/rtweekend.h
        src/interval/interval.h
        src/camera/camera.h
        src/primitives/mesh.h
        src/primitives/triangle.h
        src/material/material.h
        src/ext/tiny_obj_loader/tiny_obj_loader.h
)

target_link_libraries(raytracing_weekend PUBLIC stdc++)
target_link_libraries(raytracing_weekend PUBLIC m)

target_compile_options(raytracing_weekend PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(raytracing_weekend PRIVATE OpenMP::OpenMP_CXX bvh)