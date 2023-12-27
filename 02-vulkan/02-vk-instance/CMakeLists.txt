cmake_minimum_required(VERSION 3.27)

project(02-vk-instance CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(02-vk-instance main.cxx)

target_compile_features(02-vk-instance PRIVATE cxx_std_20)
target_include_directories(02-vk-instance PRIVATE Vulkan::Headers)
target_link_libraries(02-vk-instance PRIVATE Vulkan::Vulkan SDL3::SDL3-shared)
