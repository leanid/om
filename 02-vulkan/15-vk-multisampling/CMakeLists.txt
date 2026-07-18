cmake_minimum_required(VERSION 4.0)

project(15-vk-multisampling CXX)

find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)

om_clang_tidy_enable()

add_library(15-vk-multisampling-tinyobj)
target_sources(
    15-vk-multisampling-tinyobj
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/tinyobj.cxx")
target_link_libraries(15-vk-multisampling-tinyobj PRIVATE 15-vk-multisampling-vulkan
                                                      tinyobjloader::tinyobjloader)

add_library(15-vk-multisampling-log)
target_sources(
    15-vk-multisampling-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(15-vk-multisampling-log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(15-vk-multisampling-platform)
target_sources(
    15-vk-multisampling-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    15-vk-multisampling-platform
    PRIVATE 15-vk-multisampling-log
            15-vk-multisampling-vulkan
            15-vk-multisampling-sdl
            15-vk-multisampling-sdl-vulkan
            15-vk-multisampling-tinyobj
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(15-vk-multisampling-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(15-vk-multisampling main.cxx)
target_compile_features(15-vk-multisampling PRIVATE cxx_std_23)
target_include_directories(
    15-vk-multisampling PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    15-vk-multisampling
    PRIVATE 15-vk-multisampling-vulkan
            15-vk-multisampling-platform
            15-vk-multisampling-sdl
            15-vk-multisampling-sdl-vulkan)

# cmake-format: off
target_link_libraries(
 15-vk-multisampling
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
om_add_slang_shader_target(generate_spirv_11 SOURCES ${slang_files})

# Make game target depends on generate_spirv target
add_dependencies(15-vk-multisampling generate_spirv_11)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(
        TARGET 15-vk-multisampling
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:15-vk-multisampling>
        COMMENT
            "Copying SDL3 library to target directory (Windows only) vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()