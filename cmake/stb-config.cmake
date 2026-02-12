# stb (header-only) package config.
# Exposes imported interface target: stb::stb
find_path(stb_INCLUDE_DIR
    NAMES stb/stb_image.h
    PATHS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH)

if(NOT stb_INCLUDE_DIR)
    set(stb_FOUND FALSE)
    if(stb_FIND_REQUIRED)
        message(FATAL_ERROR
            "stb was not found. Build deps first: cd deps/rules && CXX=clang++ cmake -P linux-build.cmake")
    endif()
    return()
endif()

set(stb_FOUND TRUE)

if(NOT TARGET stb::stb)
    add_library(stb INTERFACE IMPORTED)
    set_target_properties(stb PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${stb_INCLUDE_DIR}")
    add_library(stb::stb ALIAS stb)
endif()
