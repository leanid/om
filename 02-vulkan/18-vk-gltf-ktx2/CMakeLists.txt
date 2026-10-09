cmake_minimum_required(VERSION 4.0)

project(18-vk-gltf-ktx2 CXX C)

find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)

om_clang_tidy_enable()

add_library(18-vk-gltf-ktx2-gltf)
target_sources(
    18-vk-gltf-ktx2-gltf
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/gltf.cxx")
target_link_libraries(18-vk-gltf-ktx2-gltf PRIVATE 18-vk-gltf-ktx2-vulkan
                                                  fastgltf::fastgltf)

add_library(18-vk-gltf-ktx2-log)
target_sources(
    18-vk-gltf-ktx2-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(18-vk-gltf-ktx2-log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(18-vk-gltf-ktx2-platform)
target_sources(
    18-vk-gltf-ktx2-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    18-vk-gltf-ktx2-platform
    PRIVATE 18-vk-gltf-ktx2-log
            18-vk-gltf-ktx2-vulkan
            18-vk-gltf-ktx2-sdl
            18-vk-gltf-ktx2-sdl-vulkan
            18-vk-gltf-ktx2-gltf
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(18-vk-gltf-ktx2-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(18-vk-gltf-ktx2 main.cxx)
target_compile_features(18-vk-gltf-ktx2 PRIVATE cxx_std_23)
target_include_directories(
    18-vk-gltf-ktx2 PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    18-vk-gltf-ktx2
    PRIVATE 18-vk-gltf-ktx2-vulkan
            18-vk-gltf-ktx2-platform
            18-vk-gltf-ktx2-sdl
            18-vk-gltf-ktx2-sdl-vulkan)

# cmake-format: off
target_link_libraries(
 18-vk-gltf-ktx2
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
om_add_slang_shader_target(generate_spirv_18 SOURCES ${slang_files})

set(compute_spirv "${CMAKE_CURRENT_SOURCE_DIR}/shaders/compute.slang.spv")
add_custom_command(
    OUTPUT ${compute_spirv}
    COMMAND
        slangc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/compute.slang -target spirv
        -fvk-use-entrypoint-name -entry comp_main -o ${compute_spirv} -profile
        spirv_1_4 -emit-spirv-directly -g2
    VERBATIM
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/compute.slang)

add_custom_target(generate_spirv_18_compute DEPENDS ${compute_spirv})

# Make game target depends on generate_spirv targets
add_dependencies(18-vk-gltf-ktx2 generate_spirv_18 generate_spirv_18_compute)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(
        TARGET 18-vk-gltf-ktx2
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:18-vk-gltf-ktx2>
        COMMENT
            "Copying SDL3 library to target directory (Windows only) vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()
