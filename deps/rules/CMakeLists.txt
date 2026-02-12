cmake_minimum_required(VERSION 4.2)

project(prebuilt_dependencies CXX)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/../../cmake)
find_package(om-triplet-name REQUIRED)

get_filename_component(prebuilt_dir "${CMAKE_SOURCE_DIR}/../prebuilt" ABSOLUTE)
message(STATUS "${prebuilt_dir}")

set(CMAKE_INSTALL_PREFIX "${prebuilt_dir}/${OM_TRIPLET_NAME}")
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
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DCATCH_INSTALL_DOCS=OFF
        -DCATCH_INSTALL_EXTRAS=OFF
        -DCATCH_DEVELOPMENT_BUILD=OFF
        -DCATCH_ENABLE_REPRODUCIBLE_BUILD=OFF
)
# install SDL3 dependencies from:
# https://wiki.libsdl.org/SDL3/README-linux#build-dependencies
ExternalProject_Add(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.28
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DSDL_LIBC=ON # Building with -DSDL_LIBC=OFF will make it impossible to use the sanitizer
        -DSDL_STATIC=ON
        -DSDL_SHARED=ON
        -DSDL_TESTS=OFF
        -DSDL_TEST_LIBRARY=OFF
        -DSDL_INSTALL_DOCS=OFF
)
ExternalProject_Add(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DGLM_BUILD_TESTS=OFF
        -DGLM_ENABLE_FAST_MATH=ON
        -DGLM_ENABLE_CXX_20=ON
        -DGLM_ENABLE_LANG_EXTENSIONS=OFF
        -DGLM_BUILD_LIBRARY=ON # static/dynamic
)
ExternalProject_Add(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.90.0
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} # -Wno-unused-command-line-argument
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
)
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
)
ExternalProject_Add(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DDOCTEST_WITH_TESTS=OFF
        -DDOCTEST_WITH_MAIN_IN_STATIC_LIB=ON
        -DDOCTEST_NO_INSTALL=OFF
        -DDOCTEST_USE_STD_HEADERS=ON
)
ExternalProject_Add(
    trompeloeil
    GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
    GIT_TAG v49
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DTROMPELOEIL_BUILD_TESTS=OFF
        -DTROMPELOEIL_INSTALL_DOCS=OFF
)

ExternalProject_Add(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DBUILD_SHARED_LIBS=OFF
        -DASSIMP_BUILD_TESTS=OFF
        -DASSIMP_INSTALL=ON
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
        -DASSIMP_NO_EXPORT=ON
        -DASSIMP_WARNINGS_AS_ERRORS=OFF
)

# stb header-only libraries (installs headers into include/stb)
# https://github.com/nothings/stb
set(stb_src ${CMAKE_BINARY_DIR}/stb-prefix/src/stb)
set(stb_include_dir ${CMAKE_INSTALL_PREFIX}/include/stb)
ExternalProject_Add(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    # only master branch exist no tags to specify
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND
        ${CMAKE_COMMAND} -E make_directory ${stb_include_dir}
        COMMAND ${CMAKE_COMMAND} -E copy ${stb_src}/stb_image.h ${stb_include_dir}/
        COMMAND ${CMAKE_COMMAND} -E copy ${stb_src}/stb_image_write.h ${stb_include_dir}/
        COMMAND ${CMAKE_COMMAND} -E copy ${stb_src}/stb_rect_pack.h ${stb_include_dir}/
        COMMAND ${CMAKE_COMMAND} -E copy ${stb_src}/stb_textedit.h ${stb_include_dir}/
        COMMAND ${CMAKE_COMMAND} -E copy ${stb_src}/stb_truetype.h ${stb_include_dir}/
)
