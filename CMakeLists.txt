cmake_minimum_required(VERSION 3.22...3.27)

include("cmake/HunterGate.cmake")

HunterGate(
    URL "https://github.com/cpp-pm/hunter/archive/v0.24.18.tar.gz"
    SHA1 "1292e4d661e1770d6d6ca08c12c07cf34a0bf718"
)

project(gamedev_study CXX)

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
