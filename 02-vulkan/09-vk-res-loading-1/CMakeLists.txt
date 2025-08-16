cmake_minimum_required(VERSION 4.0)

project(09-vk-res-loading-1 CXX)
find_package(Vulkan REQUIRED COMPONENTS glslangValidator)
find_package(SDL3 REQUIRED)
find_package(
    Boost 1.88.0 # should match version in root CMakeLists.txt
    EXACT # Minimum or EXACT version e.g. 1.67.0
    # REQUIRED # Fail with error if Boost is not found
    COMPONENTS program_options # Boost libraries by their canonical name
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

# set up Vulkan C++ module
add_library(vulkan_cppm)
add_library(vulkan::cppm ALIAS vulkan_cppm)

# cmake-format: off
target_compile_definitions(
    vulkan_cppm
    PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=0 # with 1 not linking
           VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
           VULKAN_HPP_ENABLE_STD_MODULE=1)
# cmake-format: on
target_include_directories(vulkan_cppm PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(vulkan_cppm PRIVATE Vulkan::Vulkan)
target_compile_features(vulkan_cppm PRIVATE cxx_std_23)

target_sources(
    vulkan_cppm
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${Vulkan_INCLUDE_DIR}"
           FILES
           "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm")

# set up GLM C++ module
add_library(glm_cppm)
add_library(glm::cppm ALIAS glm_cppm)

# cmake-format: off
target_compile_definitions(
    glm_cppm
    PUBLIC GLM_GTC_INLINE_NAMESPACE
           GLM_EXT_INLINE_NAMESPACE
           GLM_GTX_INLINE_NAMESPACE)
# cmake-format: on
target_include_directories(glm_cppm PRIVATE "${glm_INCLUDE_DIR}")
target_compile_features(glm_cppm PRIVATE cxx_std_23)

target_sources(
    glm_cppm
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/glm.cxx")

om_clang_tidy_enable()

add_library(09-vk-res-loading-1-log)
target_sources(
    09-vk-res-loading-1-log
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/log.cxx")

target_include_directories(09-vk-res-loading-1-log
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(09-vk-res-loading-1-platform)
target_sources(
    09-vk-res-loading-1-platform
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/platform_sdl3.cxx")
target_link_libraries(
    09-vk-res-loading-1-platform
    PRIVATE 09-vk-res-loading-1-log
            09-vk-res-loading-1-vulkan
            09-vk-res-loading-1-sdl
            09-vk-res-loading-1-sdl-vulkan
            SDL3::SDL3-shared
            om::io::read_file)
target_include_directories(09-vk-res-loading-1-platform
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)

add_subdirectory(sdl)
add_subdirectory(vulkan)

add_executable(09-import-test main2.cxx)
target_compile_features(09-import-test PRIVATE cxx_std_23)
target_link_libraries(09-import-test PUBLIC vulkan::cppm glm::cppm)

add_executable(09-vk-res-loading-1 main.cxx)
target_compile_features(09-vk-res-loading-1 PRIVATE cxx_std_23)
target_include_directories(
    09-vk-res-loading-1 PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                                ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    09-vk-res-loading-1
    PRIVATE 09-vk-res-loading-1-vulkan
            09-vk-res-loading-1-platform
            09-vk-res-loading-1-sdl
            09-vk-res-loading-1-sdl-vulkan)

# cmake-format: off
target_link_libraries(
09-vk-res-loading-1
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
file(GLOB_RECURSE glsl_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert
     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag)

file(GLOB_RECURSE slang_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.slang)

foreach(glsl_file ${glsl_files})
    set(spirv_file "${CMAKE_CURRENT_SOURCE_DIR}/${glsl_file}.spv")

    # Add custom command to compile shader file to SPIR-V
    add_custom_command(
        OUTPUT ${spirv_file}
        COMMAND
            Vulkan::glslangValidator -V --target-env vulkan1.0
            ${CMAKE_CURRENT_SOURCE_DIR}/${glsl_file} -o ${spirv_file}
        VERBATIM
        DEPENDS ${glsl_file})

    list(APPEND spirv_files ${spirv_file})
endforeach()

foreach(slang_file ${slang_files})
    set(spirv_file "${CMAKE_CURRENT_SOURCE_DIR}/${slang_file}.spv")

    # Add custom command to compile shader file to SPIR-V
    # cmake-format: off
    add_custom_command(
        OUTPUT ${spirv_file}
        COMMAND
            slangc ${CMAKE_CURRENT_SOURCE_DIR}/${slang_file}
            -target spirv
            #-entry main
            -o ${spirv_file}
            -profile spirv_1_3
            # Vulkan 1.1 or later is required because the GPU
            # instrumentation code uses SPIR-V 1.3 features.
            # Vulkan 1,1 is required to ensure that SPIR-V 1.3 is available.
            -emit-spirv-directly # only for RenderDoc debuging
            #-emit-spirv-via-glsl # if profile spirv_1_0
            -g2 # enable debug info in SPIR-V for RenderDoc
        VERBATIM
        DEPENDS ${slang_file})
    # cmake-format: on
    list(APPEND spirv_files ${spirv_file})
endforeach()

# Add custom target which depends on SPIR-V files
add_custom_target(generate_spirv_5 ALL DEPENDS ${spirv_files})

# Make game target depends on generate_spirv target
add_dependencies(09-vk-res-loading-1 generate_spirv_5)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 09-vk-res-loading-1
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:09-vk-res-loading-1>
        COMMENT [=[Copying SDL3 library to target directory (Windows only)\n
                vulkan-1.dll also is copied every time - cmake do it for us]=]
        VERBATIM)
endif()
