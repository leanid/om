cmake_minimum_required(VERSION 3.27)

project(04-vk-shapchain-1 CXX)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

add_executable(04-vk-shapchain-1 main.cxx)

target_compile_features(04-vk-shapchain-1 PRIVATE cxx_std_20)
target_include_directories(
    04-vk-shapchain-1
    PRIVATE
        Vulkan::Headers
        $<$<PLATFORM_ID:Windows,Darwin>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>)
target_link_libraries(04-vk-shapchain-1 PRIVATE Vulkan::Vulkan
                                                SDL3::SDL3-shared)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 04-vk-shapchain-1
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:04-vk-shapchain-1>
        COMMENT "Copying SDL3 library to target directory (Windows only)\n"
                "vulkan-1.dll also is copied every time - cmake to it for us"
        VERBATIM)
endif()
