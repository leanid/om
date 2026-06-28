cmake_minimum_required(VERSION 4.0)

project(13-vk-model CXX)

find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)

om_clang_tidy_enable()

add_library(13-vk-model-tinyobj)
target_sources(
    13-vk-model-tinyobj
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/tinyobj.cxx")
target_link_libraries(13-vk-model-tinyobj PRIVATE 13-vk-model-vulkan
                                                  tinyobjloader::tinyobjloader)

add_library(13-vk-model-log)
target_sources(
    13-vk-model-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(13-vk-model-log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(13-vk-model-platform)
target_sources(
    13-vk-model-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    13-vk-model-platform
    PRIVATE 13-vk-model-log
            13-vk-model-vulkan
            13-vk-model-sdl
            13-vk-model-sdl-vulkan
            13-vk-model-tinyobj
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(13-vk-model-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(13-vk-model main.cxx)
target_compile_features(13-vk-model PRIVATE cxx_std_23)
target_include_directories(
    13-vk-model PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    13-vk-model
    PRIVATE 13-vk-model-vulkan
            13-vk-model-platform
            13-vk-model-sdl
            13-vk-model-sdl-vulkan)

# cmake-format: off
target_link_libraries(
 13-vk-model
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
om_add_slang_shader_target(generate_spirv_9 SOURCES ${slang_files})

# Make game target depends on generate_spirv target
add_dependencies(13-vk-model generate_spirv_9)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(
        TARGET 13-vk-model
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:13-vk-model-loading-1>
        COMMENT
            "Copying SDL3 library to target directory (Windows only) vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()
