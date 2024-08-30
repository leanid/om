cmake_minimum_required(VERSION 3.27)

project(01-vk-enumerate-extensions CXX)

# on Windows you can just add $env VULKAN_SDK=C:/VulkanSDK/1.3.290.0
find_package(Vulkan REQUIRED)

add_executable(01-vk-enumerate-extensions main.cxx)

target_include_directories(01-vk-enumerate-extensions PRIVATE Vulkan::Headers)
target_link_libraries(01-vk-enumerate-extensions PRIVATE Vulkan::Vulkan)
