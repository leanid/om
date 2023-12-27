cmake_minimum_required(VERSION 3.27)
if(APPLE)
    message(STATUS "#include <experimental/scope> not supported\n"
                   " on clang++17.0.6 on mac so skip 03-vk-validation")
    return()
endif()
project(03-vk-validation CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(03-vk-validation main.cxx)

target_compile_features(03-vk-validation PRIVATE cxx_std_20)
target_include_directories(03-vk-validation PRIVATE Vulkan::Headers)
target_link_libraries(03-vk-validation PRIVATE Vulkan::Vulkan SDL3::SDL3-shared)
