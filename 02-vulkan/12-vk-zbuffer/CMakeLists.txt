cmake_minimum_required(VERSION 4.0)

project(12-vk-zbuffer CXX)

find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)

om_clang_tidy_enable()

add_library(12-vk-zbuffer-log)
target_sources(
    12-vk-zbuffer-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(12-vk-zbuffer-log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(12-vk-zbuffer-platform)
target_sources(
    12-vk-zbuffer-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    12-vk-zbuffer-platform
    PRIVATE 12-vk-zbuffer-log
            12-vk-zbuffer-vulkan
            12-vk-zbuffer-sdl
            12-vk-zbuffer-sdl-vulkan
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(12-vk-zbuffer-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(12-vk-zbuffer main.cxx)
target_compile_features(12-vk-zbuffer PRIVATE cxx_std_23)
target_include_directories(
    12-vk-zbuffer PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                                    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    12-vk-zbuffer
    PRIVATE 12-vk-zbuffer-vulkan
            12-vk-zbuffer-platform
            12-vk-zbuffer-sdl
            12-vk-zbuffer-sdl-vulkan)

# cmake-format: off
target_link_libraries(
12-vk-zbuffer
PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:
    $<IF:$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,14.0.0>,
        stdc++exp,
        $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,12.1.1>:
            stdc++_libbacktrace>
        >
    >)
# cmake-format: on

# Find the shader files
file(GLOB_RECURSE slang_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.slang)

# Add custom target which depends on SPIR-V files
om_add_slang_shader_target(generate_spirv_8 SOURCES ${slang_files})

# Make game target depends on generate_spirv target
add_dependencies(12-vk-zbuffer generate_spirv_8)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(
        TARGET 12-vk-zbuffer
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:12-vk-zbuffer-loading-1>
        COMMENT
            "Copying SDL3 library to target directory (Windows only) vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()
