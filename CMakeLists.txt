cmake_minimum_required(VERSION 3.22...3.27)

project(gamedev_study CXX)

option(OM_BUILD_DEPS "download and build dependencies as part of current build"
       ON)
option(OM_BUILD_COURSE_BASIC_PROG "turn ON if need to build 00-basic-prog" ON)
option(OM_BUILD_COURSE_BASIC_GAME_DEV
       "turn ON if need to build 01-basic-game-dev" ON)
option(OM_BUILD_COURSE_OPENGL "turn ON if need to build 02-opengl" OFF)
option(OM_BUILD_COURSE_VULKAN "turn ON if need to build 02-vulkan" ON)
option(OM_BUILD_GAME_EXAMPLE "turn ON if need to build 05-game" OFF)

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

    message(STATUS "download and build GLM sources")
    fetchcontent_declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG origin/master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE TRUE)
    fetchcontent_makeavailable(glm)
    find_package(glm REQUIRED)
endif(${OM_BUILD_DEPS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# curses
if(OM_BUILD_COURSE_BASIC_PROG)
    add_subdirectory(00-basic-prog)
endif()

if(OM_BUILD_COURSE_BASIC_GAME_DEV)
    add_subdirectory(01-basic-game-dev)
endif()

if(OM_BUILD_COURSE_OPENGL)
    add_subdirectory(02-opengl)
endif()

if(OM_BUILD_COURSE_VULKAN)
    add_subdirectory(02-vulkan)
endif()

# start sample project
if(OM_BUILD_GAME_EXAMPLE)
    add_subdirectory(05-game)
endif(OM_BUILD_GAME_EXAMPLE)
