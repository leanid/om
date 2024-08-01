cmake_minimum_required(VERSION 3.16)
project(03-sdl-loop-to-engine)

add_executable(03-sdl-loop-to-engine engine.cxx engine.hxx main.cxx)
target_compile_features(03-sdl-loop-to-engine PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

target_link_libraries(03-sdl-loop-to-engine PRIVATE SDL3::SDL3-shared)
