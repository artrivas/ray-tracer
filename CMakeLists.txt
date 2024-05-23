cmake_minimum_required(VERSION 3.28)
project(raytracing_weekend)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_STANDARD 20)
set(embree_DIR ${CMAKE_SOURCE_DIR}/src/ext/embree/lib/cmake/embree-4.3.1)

add_library(tiny_obj_loader STATIC src/ext/tiny_obj_loader/tiny_obj_loader.cc)
target_include_directories(tiny_obj_loader PUBLIC ${CMAKE_SOURCE_DIR}/src/ext/tiny_obj_loader)
find_package(embree 4 REQUIRED)
add_subdirectory(src/ext/bvh)

add_executable(raytracing_weekend src/main.cpp
        src/vec3/vec3.h
        src/color/color.h
        src/ray/ray.h
        src/hittable/hitabble.h
        src/hittable/hittable_list.h
        src/rtweekend.h
        src/interval/interval.h
        src/camera/camera.h
        src/primitives/mesh.h
        src/material/material.h
        src/accelerator/bvh.h
        src/texture/texture.h
        src/perlin/perlin.h
        src/primitives/quads.h
        src/scene/scene.h
)
target_link_libraries(raytracing_weekend PRIVATE tiny_obj_loader embree)

target_link_libraries(raytracing_weekend PUBLIC stdc++)
target_link_libraries(raytracing_weekend PUBLIC m)

target_compile_options(raytracing_weekend PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(raytracing_weekend PRIVATE OpenMP::OpenMP_CXX bvh)
