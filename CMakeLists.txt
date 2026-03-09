cmake_minimum_required(VERSION 4.2)

option(OM_BASIC_GAME_DEV "turn ON to build 01-basic-game-dev" ON)
option(OM_BASIC_PROG "turn ON if need to build 00-basic-prog" ON)
option(OM_OPENGL "turn ON if need to build 02-opengl" ON)
option(OM_VULKAN "turn ON if need to build 02-vulkan" ON)
option(OM_GAME_EXAMPLE "turn ON if need to build 05-game" OFF)
option(OM_WEB "turn ON if need to build 08-web" ON)
option(OM_CLANG_TIDY "turn ON if need to run clang-tidy" ON)

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

project(
    rgd
    VERSION 0.1.0
    # SPDX_LICENSE "Zlib"
    DESCRIPTION
        "RGD - real game dev study project. \
This is a collection of tutorials about basic programming in game dev with \
modern C++. Simple bash tutorials, C++ terminal progamming, Input, \
Graphics, OpenGL, Vulkan, Audio programming. Cross-platform \
linux/windows/macos/ios/android developing. All with modern C++."
    HOMEPAGE_URL "https://github.com/leanid/om"
    LANGUAGES CXX)

list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(om-common-functions REQUIRED)
om_add_custom_build_types()

find_package(om-triplet-name REQUIRED)
list(PREPEND CMAKE_PREFIX_PATH
     ${CMAKE_SOURCE_DIR}/deps/prebuilt/${OM_TRIPLET_NAME})

find_package(Boost COMPONENTS program_options stacktrace REQUIRED)
find_package(Catch2 REQUIRED)
find_package(GTest REQUIRED)
find_package(SDL3 REQUIRED)
find_package(Vulkan REQUIRED COMPONENTS glslangValidator)
find_package(doctest REQUIRED)
find_package(glm REQUIRED)
find_package(trompeloeil REQUIRED)
find_package(stb REQUIRED)
find_package(assimp REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(redis++ REQUIRED)

enable_testing()

om_clang_tidy_enable()

# courses
if(OM_BASIC_PROG)
    add_subdirectory(00-basic-prog)
endif()

if(OM_BASIC_GAME_DEV)
    add_subdirectory(01-basic-game-dev)
endif()

if(OM_OPENGL)
    add_subdirectory(02-opengl)
endif()

if(OM_VULKAN)
    add_subdirectory(02-vulkan)
endif()

if(OM_GAME_EXAMPLE)
    add_subdirectory(05-game)
endif()

if(OM_WEB)
    add_subdirectory(08-web)
endif()
