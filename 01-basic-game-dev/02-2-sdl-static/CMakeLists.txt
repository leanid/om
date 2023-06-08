cmake_minimum_required(VERSION 3.20)
project(02-sdl-static)

add_executable(02-sdl-static main.cxx)
target_compile_features(02-sdl-static PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)
#target_include_directories(02-sdl-static PRIVATE ${SDL2_INCLUDE_DIRS})
# We want to link with our own debug version on SDL2 lib statically
# So before try it you have to build locally SDL2 and install it (usually to /usr/local)
#find_library(SDL2_LIB libSDL2d.a)
## if you need to link with our own compiled library you may point cmake to it.
#find_library(SDL2_LIB
#             NAMES libSDL2d.a
#             PATHS /usr/local /usr/local/lib /usr/local/lib
#             )
# if you just want to install system package with static version of SDL2 libs
# >sudo dnf install SDL2-static (on Fedora)

if(TARGET SDL3::SDL3-static)
    get_target_property(PATH_TO_SDL3_STATIC SDL3::SDL3-static LOCATION)
    message(STATUS "path to static libSDL3.a [${PATH_TO_SDL3_STATIC}]")
else()
    message(FATAL_ERROR "Error: find_library(...) did not find libSDL3.a")
endif()

target_link_libraries(02-sdl-static PRIVATE SDL3::SDL3-static)
set_property(TARGET 02-sdl-static PROPERTY POSITION_INDEPENDENT_CODE TRUE)
