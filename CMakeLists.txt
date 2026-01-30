cmake_minimum_required(VERSION 4.2)
# Enables the Standard module support. This needs to be done
# before selecting the languages.
# https://github.com/Kitware/CMake/blob/master/Help/dev/experimental.rst
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
# Currently CMake requires extensions enabled when using import std.
# https://gitlab.kitware.com/cmake/cmake/-/issues/25916
# https://gitlab.kitware.com/cmake/cmake/-/issues/25539
set(CMAKE_CXX_EXTENSIONS OFF) # no -std=gnu-23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Tell CMake that we explicitly want `import std`. This will initialize the
# property on all targets declared after this to 1
set(CMAKE_CXX_MODULE_STD 1)

project(rgd LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(om-triplet-name REQUIRED)
find_package(om-common-functions REQUIRED)
list(APPEND CMAKE_PREFIX_PATH
     ${CMAKE_SOURCE_DIR}/deps/prebuilt/${OM_TRIPLET_NAME})

om_add_custom_build_types()

option(OM_BUILD_DEPS [=[download and build dependencies [SDL3, GLM, Catch2]
    as part of current build]=] OFF)
option(OM_BUILD_BOOST "download and build Boost from source" OFF)
option(OM_BUILD_COURSE_BASIC_PROG "turn ON if need to build 00-basic-prog" ON)
option(OM_BUILD_COURSE_BASIC_GAME_DEV
       "turn ON if need to build 01-basic-game-dev" ON)
option(OM_BUILD_COURSE_OPENGL "turn ON if need to build 02-opengl" OFF)
option(OM_BUILD_COURSE_VULKAN "turn ON if need to build 02-vulkan" ON)
option(OM_BUILD_GAME_EXAMPLE "turn ON if need to build 05-game" OFF)
option(OM_BUILD_WEB "turn ON if need to build 08-web" ON)
option(OM_CLANG_TIDY "turn ON if need to run clang-tidy" ON)

find_package(Catch2 REQUIRED)
find_package(GTest REQUIRED)
find_package(doctest REQUIRED)
find_package(trompeloeil REQUIRED)
find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)
find_package(
    Boost
    1.90.0
    EXACT # Minimum or EXACT version e.g. 1.67.0
    REQUIRED # Fail with error if Boost is not found
    COMPONENTS filesystem stacktrace
               program_options # Boost libraries by their canonical name
    # e.g. "date_time" for "libboost_date_time"
    #[OPTIONAL_COMPONENTS <libs>...]
    # Optional Boost libraries by their canonical name)
    ) # e.g. "date_time" for "libboost_date_time"

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
