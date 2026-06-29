cmake_minimum_required(VERSION 4.0)

project(14-vk-mipmaps CXX)

find_program(SLANGC_EXECUTABLE slangc HINTS "$ENV{VULKAN_SDK}/bin" REQUIRED)

om_clang_tidy_enable()

add_library(14-vk-mipmaps-tinyobj)
target_sources(
    14-vk-mipmaps-tinyobj
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/tinyobj.cxx")
target_link_libraries(14-vk-mipmaps-tinyobj PRIVATE 14-vk-mipmaps-vulkan
                                                      tinyobjloader::tinyobjloader)

add_library(14-vk-mipmaps-log)
target_sources(
    14-vk-mipmaps-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(14-vk-mipmaps-log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(14-vk-mipmaps-platform)
target_sources(
    14-vk-mipmaps-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    14-vk-mipmaps-platform
    PRIVATE 14-vk-mipmaps-log
            14-vk-mipmaps-vulkan
            14-vk-mipmaps-sdl
            14-vk-mipmaps-sdl-vulkan
            14-vk-mipmaps-tinyobj
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(14-vk-mipmaps-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(14-vk-mipmaps main.cxx)
target_compile_features(14-vk-mipmaps PRIVATE cxx_std_23)
target_include_directories(
    14-vk-mipmaps PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                        ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    14-vk-mipmaps
    PRIVATE 14-vk-mipmaps-vulkan
            14-vk-mipmaps-platform
            14-vk-mipmaps-sdl
            14-vk-mipmaps-sdl-vulkan)

# cmake-format: off
target_link_libraries(
 14-vk-mipmaps
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
om_add_slang_shader_target(generate_spirv_10 SOURCES ${slang_files})

# Make game target depends on generate_spirv target
add_dependencies(14-vk-mipmaps generate_spirv_10)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(
        TARGET 14-vk-mipmaps
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:14-vk-mipmaps-loading-1>
        COMMENT
            "Copying SDL3 library to target directory (Windows only) vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()