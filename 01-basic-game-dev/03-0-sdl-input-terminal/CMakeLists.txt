cmake_minimum_required(VERSION 3.20..3.22)
project(03-0-sdl-input-terminal)

add_executable(03-0-sdl-input-terminal main.cxx)
target_compile_features(03-0-sdl-input-terminal PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

target_link_libraries(03-0-sdl-input-terminal PRIVATE SDL3::SDL3)
