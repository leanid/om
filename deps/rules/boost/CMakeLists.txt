cmake_minimum_required(VERSION 4.2)

project(prebuild_boost_installer CXX)

string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" compiler_name)
# Извлекаем мажорную версию (первое число до точки)
# CMAKE_CXX_COMPILER_VERSION содержит "13.2.0" -> станет "13"
string(REGEX REPLACE "^([0-9]+).*$" "\\1" compiler_major_version "${CMAKE_CXX_COMPILER_VERSION}")

set(compiler_full_id "${compiler_name}${compiler_major_version}")

message(STATUS "OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "compiler: ${compiler_full_id}")
message(STATUS "architecture: ${CMAKE_SYSTEM_PROCESSOR}")
set(triplet_name "${CMAKE_SYSTEM_NAME}-${compiler_full_id}-${CMAKE_SYSTEM_PROCESSOR}")
string(TOLOWER "${triplet_name}" triplet_name)
message(STATUS "triplet OS-compiler-architecture: ${triplet_name}")

get_filename_component(prebuilt_dir "${CMAKE_SOURCE_DIR}/../../prebuilt" ABSOLUTE)
message(STATUS "${prebuilt_dir}")

set(CMAKE_INSTALL_PREFIX "${prebuilt_dir}/${triplet_name}")
message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")

include(ExternalProject)
# Это заставит внешний проект установиться СРАЗУ во время сборки (build time)
# в общую папку установки.
ExternalProject_Add(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
    # Передаем префикс установки вашего основного проекта во внешний
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCATCH_INSTALL_DOCS=OFF
        -DCATCH_INSTALL_EXTRAS=OFF
        -DCATCH_DEVELOPMENT_BUILD=OFF
        -DCATCH_ENABLE_REPRODUCIBLE_BUILD=OFF
)
ExternalProject_Add(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG origin/main
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DSDL_STATIC=ON
        -DSDL_SHARED=OFF
        -DSDL_X11_XTEST=OFF
)
ExternalProject_Add(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG origin/master
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
)
