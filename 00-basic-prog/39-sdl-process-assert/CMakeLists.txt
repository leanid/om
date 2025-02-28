cmake_minimum_required(VERSION 3.31)

project(39-sdl-process-assert)

find_package(SDL3 REQUIRED)

add_executable(39-sdl-process-assert main.cxx show_message.cxx show_message.hxx)

target_link_libraries(39-sdl-process-assert PRIVATE SDL3::SDL3)
