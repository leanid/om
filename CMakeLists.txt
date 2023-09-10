cmake_minimum_required(VERSION 3.22...3.27)

project(gamedev_study CXX)

option(OM_BUILD_DEPS "download and build dependencies as part of current build"
       ON)

if(${OM_BUILD_DEPS})
    message(STATUS "download and build SDL3 sources")
    include(FetchContent)
    fetchcontent_declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG origin/main
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE TRUE)
    option(SDL_STATIC "" ON)
    option(SDL_SHARED "" ON)
    fetchcontent_makeavailable(SDL3)
    #fetchcontent_populate(SDL3)
    find_package(SDL3 REQUIRED)
endif(${OM_BUILD_DEPS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(OM_BUILD_COURSE_BASIC_PROG ON
    CACHE BOOL "turn ON if need to build 00-basic-prog")
set(OM_BUILD_COURSE_BASIC_GAME_DEV ON
    CACHE BOOL "turn ON if need to build 01-basic-game-dev")
set(OM_BUILD_COURSE_GRAPHICS_3D OFF
    CACHE BOOL "turn ON if need to build 02-graphics-3d")
set(OM_BUILD_GAME_EXAMPLE OFF CACHE BOOL "turn ON if need to build 05-game")

set(CMAKE_CXX_EXTENSIONS OFF)

# curses
if(${OM_BUILD_COURSE_BASIC_PROG})
    add_subdirectory(00-basic-prog)
endif(${OM_BUILD_COURSE_BASIC_PROG})

if(${OM_BUILD_COURSE_BASIC_GAME_DEV})
    add_subdirectory(01-basic-game-dev)
endif(${OM_BUILD_COURSE_BASIC_GAME_DEV})

if(${OM_BUILD_COURSE_GRAPHICS_3D})
    add_subdirectory(02-graphics-3d)
endif(${OM_BUILD_COURSE_GRAPHICS_3D})
# start sample project
if(${OM_BUILD_GAME_EXAMPLE})
    add_subdirectory(05-game)
endif(${OM_BUILD_GAME_EXAMPLE})
