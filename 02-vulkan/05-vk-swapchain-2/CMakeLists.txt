cmake_minimum_required(VERSION 3.27)

project(05-vk-swapchain-2 CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(05-vk-swapchain-2 main.cxx)

target_compile_definitions(05-vk-swapchain-2 PRIVATE VK_NULL_HANDLE=nullptr)
target_compile_features(05-vk-swapchain-2 PRIVATE cxx_std_23)
target_include_directories(
    05-vk-swapchain-2
    PRIVATE
        $<$<PLATFORM_ID:Windows,Darwin,Linux>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>
    )
target_link_libraries(05-vk-swapchain-2 PRIVATE Vulkan::Headers Vulkan::Vulkan
                                                SDL3::SDL3-shared)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 05-vk-swapchain-2
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:05-vk-swapchain-2>
        COMMENT "Copying SDL3 library to target directory (Windows only)\n"
                "vulkan-1.dll also is copied every time - cmake to it for us"
        VERBATIM)
endif()
