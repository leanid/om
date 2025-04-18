cmake_minimum_required(VERSION 3.30)

project(08-vk-framebuffer-cmd-1 CXX)

find_package(Vulkan REQUIRED COMPONENTS glslangValidator)
find_package(SDL3 REQUIRED)

om_clang_tidy_enable()

add_executable(
    08-vk-framebuffer-cmd-1
    main.cxx
    platform_sdl3.cxx
    platform_sdl3.hxx
    render.cxx
    render.hxx)
target_compile_features(08-vk-framebuffer-cmd-1 PRIVATE cxx_std_23)
target_include_directories(
    08-vk-framebuffer-cmd-1
    PRIVATE
        $<$<PLATFORM_ID:Windows,Darwin,Linux>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>
    )
target_link_libraries(
    08-vk-framebuffer-cmd-1
    PRIVATE om::io::read_file
            Vulkan::Headers
            Vulkan::Vulkan
            SDL3::SDL3-shared)

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
add_custom_target(generate_spirv_3 ALL DEPENDS ${spirv_files})

# Make game target depends on generate_spirv target
add_dependencies(08-vk-framebuffer-cmd-1 generate_spirv_3)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET 08-vk-framebuffer-cmd-1
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:08-vk-framebuffer-cmd-1>
        COMMENT [=[Copying SDL3 library to target directory (Windows only)\n
                vulkan-1.dll also is copied every time - cmake do it for us]=]
        VERBATIM)
endif()
