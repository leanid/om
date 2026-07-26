cmake_minimum_required(VERSION 4.0)

project(16-vk-compute CXX)

find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)

om_clang_tidy_enable()

add_library(16-vk-compute-tinyobj)
target_sources(
    16-vk-compute-tinyobj
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/tinyobj.cxx")
target_link_libraries(16-vk-compute-tinyobj PRIVATE 16-vk-compute-vulkan
                                                      tinyobjloader::tinyobjloader)

add_library(16-vk-compute-log)
target_sources(
    16-vk-compute-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(16-vk-compute-log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(16-vk-compute-platform)
target_sources(
    16-vk-compute-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    16-vk-compute-platform
    PRIVATE 16-vk-compute-log
            16-vk-compute-vulkan
            16-vk-compute-sdl
            16-vk-compute-sdl-vulkan
            16-vk-compute-tinyobj
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(16-vk-compute-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(16-vk-compute main.cxx)
target_compile_features(16-vk-compute PRIVATE cxx_std_23)
target_include_directories(
    16-vk-compute PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    16-vk-compute
    PRIVATE 16-vk-compute-vulkan
            16-vk-compute-platform
            16-vk-compute-sdl
            16-vk-compute-sdl-vulkan)

# cmake-format: off
target_link_libraries(
 16-vk-compute
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
list(FILTER slang_files EXCLUDE REGEX "compute\\.slang$")

# Add custom target which depends on SPIR-V files (graphics shaders)
om_add_slang_shader_target(generate_spirv_16 SOURCES ${slang_files})

set(compute_spirv "${CMAKE_CURRENT_SOURCE_DIR}/shaders/compute.slang.spv")
add_custom_command(
    OUTPUT ${compute_spirv}
    COMMAND
        slangc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/compute.slang -target spirv
        -fvk-use-entrypoint-name -entry comp_main -o ${compute_spirv} -profile
        spirv_1_4 -emit-spirv-directly -g2
    VERBATIM
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/compute.slang)

add_custom_target(generate_spirv_16_compute DEPENDS ${compute_spirv})

# Make game target depends on generate_spirv targets
add_dependencies(16-vk-compute generate_spirv_16 generate_spirv_16_compute)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(
        TARGET 16-vk-compute
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:16-vk-compute>
        COMMENT
            "Copying SDL3 library to target directory (Windows only) vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()