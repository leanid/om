cmake_minimum_required(VERSION 3.27...3.29)

project(07-vk-pipeline-1 CXX)

find_package(Vulkan REQUIRED COMPONENTS glslangValidator)
find_package(SDL3 REQUIRED)

add_executable(
    07-vk-pipeline-1
    files.cxx
    files.hxx
    gfx.cxx
    gfx.hxx
    main.cxx)

target_compile_features(07-vk-pipeline-1 PRIVATE cxx_std_23)
target_include_directories(
    07-vk-pipeline-1
    PRIVATE
        Vulkan::Headers
        $<$<PLATFORM_ID:Windows,Darwin>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>)
target_link_libraries(07-vk-pipeline-1 PRIVATE Vulkan::Vulkan SDL3::SDL3-shared)

# Find the shader files
file(GLOB_RECURSE shader_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert
     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag)

# For each shader file
foreach(shader_file ${shader_files})
    set(spirv_file "${CMAKE_CURRENT_SOURCE_DIR}/${shader_file}.spv")

    # Add custom command to compile shader file to SPIR-V
    add_custom_command(
        OUTPUT ${spirv_file}
        COMMAND
            Vulkan::glslangValidator -V
            ${CMAKE_CURRENT_SOURCE_DIR}/${shader_file} -o ${spirv_file}
        DEPENDS ${shader_file})

    list(APPEND spirv_files ${spirv_file})
endforeach()

# Add custom target which depends on SPIR-V files
add_custom_target(generate_spirv ALL DEPENDS ${spirv_files})

# Make game target depends on generate_spirv target
add_dependencies(07-vk-pipeline-1 generate_spirv)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 07-vk-pipeline-1
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:07-vk-pipeline-1>
        COMMENT "Copying SDL3 library to target directory (Windows only)\n"
                "vulkan-1.dll also is copied every time - cmake do it for us"
        VERBATIM)
endif()
