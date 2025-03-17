cmake_minimum_required(VERSION 3.31)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(om-common-functions REQUIRED)
# Enables the Standard module support. This needs to be done
# before selecting the languages.
# https://github.com/Kitware/CMake/blob/master/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")

project("gamedev_study" LANGUAGES CXX)
om_add_custom_build_types()

# Tell CMake that we explicitly want `import std`. This will initialize the
# property on all targets declared after this to 1
set(CMAKE_CXX_MODULE_STD 1)

if(MSVC OR APPLE)
    set(CMAKE_CXX_STANDARD 23) # to get c++latest flag
else()
    set(CMAKE_CXX_STANDARD 23)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Currently CMake requires extensions enabled when using import std.
# https://gitlab.kitware.com/cmake/cmake/-/issues/25916
# https://gitlab.kitware.com/cmake/cmake/-/issues/25539
set(CMAKE_CXX_EXTENSIONS ON)

option(OM_BUILD_DEPS [=[download and build dependencies [SDL3, GLM, Catch2]
    as part of current build]=] ON)
option(OM_BUILD_BOOST "download and build Boost from source" OFF)
option(OM_BUILD_COURSE_BASIC_PROG "turn ON if need to build 00-basic-prog" ON)
option(OM_BUILD_COURSE_BASIC_GAME_DEV
       "turn ON if need to build 01-basic-game-dev" ON)
option(OM_BUILD_COURSE_OPENGL "turn ON if need to build 02-opengl" OFF)
option(OM_BUILD_COURSE_VULKAN "turn ON if need to build 02-vulkan" ON)
option(OM_BUILD_GAME_EXAMPLE "turn ON if need to build 05-game" OFF)
option(OM_BUILD_WEB "turn ON if need to build 08-web" ON)
option(OM_CLANG_TIDY "turn ON if need to run clang-tidy" OFF)

if(OM_BUILD_DEPS)
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
    set(CMAKE_UNITY_BUILD_MODE "GROUP")
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

    # use Catch2::Catch2 or Catch2::Catch2WithMain
    fetchcontent_declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0 # or a later release
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE TRUE # to find correct version
        EXCLUDE_FROM_ALL)
    fetchcontent_makeavailable(Catch2)
    find_package(Catch2 REQUIRED)

endif(OM_BUILD_DEPS)

if(OM_BUILD_BOOST)
    # The logging output during population can be quite verbose,
    # making the configure stage quite noisy. This cache option
    # (ON by default) hides all population output unless an error is encountered.
    # If experiencing problems with hung downloads, temporarily
    # switching this option off may help diagnose which content population
    # is causing the issue.
    set(FETCHCONTENT_QUIET OFF)

    fetchcontent_declare(
        Boost
        GIT_REPOSITORY "https://github.com/boostorg/boost.git"
        GIT_TAG "boost-1.86.0"
        GIT_PROGRESS ON
        GIT_SHALLOW ON
        OVERRIDE_FIND_PACKAGE TRUE # needed to find correct Boost
        EXCLUDE_FROM_ALL)

    fetchcontent_makeavailable(Boost)
    find_package(
        Boost
        1.86.0
        EXACT # Minimum or EXACT version e.g. 1.67.0
        REQUIRED # Fail with error if Boost is not found
        COMPONENTS filesystem # Boost libraries by their canonical name
        # e.g. "date_time" for "libboost_date_time"
        #[OPTIONAL_COMPONENTS <libs>...]
        # Optional Boost libraries by their canonical name)
        ) # e.g. "date_time" for "libboost_date_time"
endif(OM_BUILD_BOOST)

set(CMAKE_UNITY_BUILD_MODE "BATCH")

enable_testing()

om_clang_tidy_enable()

# courses
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

if(OM_BUILD_WEB)
    add_subdirectory(08-web)
endif(OM_BUILD_WEB)
