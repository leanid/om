cmake_minimum_required(VERSION 3.31)

# just right after top project(..) command
# you may add custom build types
function(om_add_custom_build_types)
    # Only modify config details if the top level project
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
        if(NOT is_multi_config)
            set_property(
                CACHE CMAKE_BUILD_TYPE
                PROPERTY STRINGS
                         Debug
                         Release
                         RelWithDebInfo
                         MinSizeRel
                         Profile)
        elseif(
            NOT
            "Profile"
            IN_LIST
            CMAKE_CONFIGURATION_TYPES)
            list(APPEND CMAKE_CONFIGURATION_TYPES Profile)
        endif()
        # Set Profile-specific flag variables (see below)...
        set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_EXE_LINKER_FLAGS_PROFILE
            "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_SHARED_LINKER_FLAGS_PROFILE
            "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_STATIC_LINKER_FLAGS_PROFILE
            "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO}")
        set(CMAKE_MODULE_LINKER_FLAGS_PROFILE
            "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} -p")

        set(CMAKE_PROFILE_POSTFIX _profile)
    else()
        message(
            FATAL_ERROR "only in top level CMakeLists.txt after project(...)"
                        " command you should add your custom build types")
    endif()
endfunction()

# should be called just after project(...) before any targets
function(om_clang_tidy_enable)
    if(OM_CLANG_TIDY)
        find_program(tidy_binary clang-tidy REQUIRED)
        set(CMAKE_CXX_CLANG_TIDY ${tidy_binary}
                                 -warnings-as-errors=-*,bugprone-* --fix
            PARENT_SCOPE)
    endif()
endfunction()

function(om_clang_tidy_disable)
    if(OM_CLANG_TIDY)
        unset(CMAKE_CXX_CLANG_TIDY PARENT_SCOPE)
    endif()
endfunction()

# example:
# ```cmake
# om_add_slang_shader_target(shader_trg SOURCES file01.slang file02.slang)
# ```
function(om_add_slang_shader_target TARGET)
    # cmake-format: off
    cmake_parse_arguments(
        PARSE_ARGV 1 # skip TARGET name
        "shader" # prefix
        "" # options
        "" # one_value_keywords
        "SOURCES" # multi_value_keywords
        )
    # cmake-format: on

    foreach(slang_file ${shader_SOURCES})
        set(spirv_file "${CMAKE_CURRENT_SOURCE_DIR}/${slang_file}.spv")

        # Add custom command to compile shader file to SPIR-V
        # cmake-format: off
        add_custom_command(
            OUTPUT ${spirv_file}
            COMMAND
                slangc ${CMAKE_CURRENT_SOURCE_DIR}/${slang_file}
                -target spirv
                -fvk-use-entrypoint-name -entry main_vert -entry main_frag
                -o ${spirv_file}
                -profile spirv_1_4
                # Vulkan 1.1 or later is required because the GPU
                # instrumentation code uses SPIR-V 1.3 features.
                # Vulkan 1.1 is required to ensure that SPIR-V 1.3 is available.
                -emit-spirv-directly # only for RenderDoc debuging
                #-emit-spirv-via-glsl # if profile spirv_1_0
                -g2 # enable debug info in SPIR-V for RenderDoc
            VERBATIM
            DEPENDS ${slang_file})
        # cmake-format: on
        list(APPEND spirv_files ${spirv_file})
    endforeach()

    add_custom_target(${TARGET} DEPENDS ${spirv_files})
endfunction()
