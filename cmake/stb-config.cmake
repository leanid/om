# stb (header-only) package config.
# Exposes imported interface target: stb::stb
find_path(
    stb_INCLUDE_DIR
    NAMES stb/stb_image.h
    # NOTE: CMake does NOT materialize the CMAKE_PREFIX_PATH environment
    # variable into the CMake variable, so consult both (find_package does
    # the same natively). The env way is used by the Docker CI image.
    PATHS ${CMAKE_PREFIX_PATH} $ENV{CMAKE_PREFIX_PATH}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH)

if(NOT stb_INCLUDE_DIR)
    set(stb_FOUND FALSE)
    if(stb_FIND_REQUIRED)
        message(
            FATAL_ERROR
                "stb was not found. Build deps first: cd deps/rules && CXX=clang++ cmake -P linux-build.cmake"
            )
    endif()
    return()
endif()

set(stb_FOUND TRUE)

if(NOT TARGET stb::stb)
    add_library(stb::stb INTERFACE IMPORTED)
    target_include_directories(stb::stb INTERFACE "${stb_INCLUDE_DIR}")
endif()
