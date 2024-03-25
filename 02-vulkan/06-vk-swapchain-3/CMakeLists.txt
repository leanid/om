cmake_minimum_required(VERSION 3.27)

project(06-vk-swapchain-3 CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(06-vk-swapchain-3 gfx.cxx gfx.hxx main.cxx)

target_compile_features(06-vk-swapchain-3 PRIVATE cxx_std_23)
target_include_directories(
    06-vk-swapchain-3
    PRIVATE
        Vulkan::Headers
        $<$<PLATFORM_ID:Windows,Darwin>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>)
target_link_libraries(06-vk-swapchain-3 PRIVATE Vulkan::Vulkan
                                                SDL3::SDL3-shared)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 06-vk-swapchain-3
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:06-vk-swapchain-3>
        COMMENT "Copying SDL3 library to target directory (Windows only)\n"
                "vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()
