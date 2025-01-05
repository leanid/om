cmake_minimum_required(VERSION 3.31)

# just right after top project(..) command
# you may add custom build types
function (om_add_custom_build_types)
    # Only modify config details if the top level project
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
        if(NOT is_multi_config)
            set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
                STRINGS Debug Release RelWithDebInfo MinSizeRel Profile
            )
        elseif(NOT "Profile" IN_LIST CMAKE_CONFIGURATION_TYPES)
            list(APPEND CMAKE_CONFIGURATION_TYPES Profile)
        endif()
        # Set Profile-specific flag variables (see below)...
        set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_EXE_LINKER_FLAGS_PROFILE "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} -p")
        set(CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO}")
        set(CMAKE_MODULE_LINKER_FLAGS_PROFILE "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} -p")

        set(CMAKE_PROFILE_POSTFIX _profile)
    else()
        message(FATAL_ERROR "only in top level CMakeLists.txt after project(...)"
                            " command you should add your custom build types")
    endif()
endfunction()

# should be called just after project(...) before any targets
function (om_clang_tidy_enable)
    find_program(tidy_binary clang-tidy REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY ${tidy_binary} -warnings-as-errors=-*,bugprone-* --fix PARENT_SCOPE)
endfunction()
