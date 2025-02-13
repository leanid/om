cmake_minimum_required(VERSION 3.27)
if(APPLE)
    message(STATUS "#include <experimental/scope> not supported\n"
                   " on clang++17.0.6 on mac so skip 03-vk-validation")
    return()
endif()
project(03-vk-validation CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

# To check validation is working replace 1 with 0  in (main.cxx) line:
#
#        device_create_info.queueCreateInfoCount    = 1;
# end you will see error line below

# VUID-VkDeviceCreateInfo-queueCreateInfoCount-arraylength(ERROR / SPEC):
# msgNum: 1641005941 -
# Validation Error:
# [ VUID-VkDeviceCreateInfo-queueCreateInfoCount-arraylength ]
# | MessageID = 0x61cfc375 |
# vkCreateDevice():
# pCreateInfo->queueCreateInfoCount must be greater than 0.
# The Vulkan spec states: queueCreateInfoCount must be greater than 0
# (https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#VUID-VkDeviceCreateInfo-queueCreateInfoCount-arraylength)
#    Objects: 0

add_executable(03-vk-validation main.cxx)
target_compile_features(03-vk-validation PRIVATE cxx_std_20)
target_include_directories(
    03-vk-validation
    PRIVATE
        $<$<PLATFORM_ID:Windows,Linux,Darvin>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>
    )
target_link_libraries(03-vk-validation PRIVATE Vulkan::Headers Vulkan::Vulkan
                                               SDL3::SDL3-shared)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 03-vk-validation
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:03-vk-validation>
        COMMENT [=["Copying SDL3 library to target directory (Windows only)\n"
                "vulkan-1.dll also is copied every time - cmake to it for us"]=]
        VERBATIM)
endif()
