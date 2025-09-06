cmake_minimum_required(VERSION 4.0)

project(11-vk-texture-loading-1 CXX)
find_package(Vulkan REQUIRED COMPONENTS glslangValidator)
find_package(SDL3 REQUIRED)
find_package(
    Boost 1.89.0 # should match version in root CMakeLists.txt
    EXACT # Minimum or EXACT version e.g. 1.67.0
    # REQUIRED # Fail with error if Boost is not found
    COMPONENTS program_options
               stacktrace # Boost libraries by their canonical name
    # REQUIRED
    # e.g. "date_time" for "libboost_date_time"
    #[OPTIONAL_COMPONENTS <libs>...]
    # Optional Boost libraries by their canonical name)
    ) # e.g. "date_time" for "libboost_date_time"
find_package(glm REQUIRED)
find_package(
    slang
    CONFIG
    HINTS
    "$ENV{VULKAN_SDK}/lib/cmake"
    REQUIRED)

om_clang_tidy_enable()

add_library(11-vk-texture-log)
target_sources(
    11-vk-texture-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(11-vk-texture-log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(11-vk-texture-platform)
target_sources(
    11-vk-texture-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    11-vk-texture-platform
    PRIVATE 11-vk-texture-log
            11-vk-texture-vulkan
            11-vk-texture-sdl
            11-vk-texture-sdl-vulkan
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(11-vk-texture-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(11-vk-texture-loading-1 main.cxx)
target_compile_features(11-vk-texture-loading-1 PRIVATE cxx_std_23)
target_include_directories(
    11-vk-texture-loading-1 PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                                ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    11-vk-texture-loading-1
    PRIVATE 11-vk-texture-vulkan
            11-vk-texture-platform
            11-vk-texture-sdl
            11-vk-texture-sdl-vulkan)

# cmake-format: off
target_link_libraries(
11-vk-texture-loading-1
PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:
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
om_add_slang_shader_target(generate_spirv_7 SOURCES ${slang_files})

# Make game target depends on generate_spirv target
add_dependencies(11-vk-texture-loading-1 generate_spirv_7)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 11-vk-texture-loading-1
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:11-vk-texture-loading-1>
        COMMENT [=[Copying SDL3 library to target directory (Windows only)\n
                vulkan-1.dll also is copied every time - cmake do it for us]=]
        VERBATIM)
endif()
