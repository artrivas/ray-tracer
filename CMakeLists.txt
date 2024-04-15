cmake_minimum_required(VERSION 3.26)
project(raytracing_weekend)

set(CMAKE_CXX_STANDARD 17)

add_executable(raytracing_weekend src/main.cpp
        src/vec3/vec3.h
        src/color/color.h
        src/ray/ray.h)
