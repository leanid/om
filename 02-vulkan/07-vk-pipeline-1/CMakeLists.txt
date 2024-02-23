cmake_minimum_required(VERSION 3.27)

project(07-vk-pipeline-1 CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(07-vk-pipeline-1 gfx.cxx gfx.hxx main.cxx)

target_compile_features(07-vk-pipeline-1 PRIVATE cxx_std_23)
target_include_directories(
    07-vk-pipeline-1
    PRIVATE
        Vulkan::Headers
        $<$<PLATFORM_ID:Windows,Darwin>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>)
target_link_libraries(07-vk-pipeline-1 PRIVATE Vulkan::Vulkan SDL3::SDL3-shared)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 07-vk-pipeline-1
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:07-vk-pipeline-1>
        COMMENT "Copying SDL3 library to target directory (Windows only)\n"
                "vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()
