cmake_minimum_required(VERSION 4.4)

project(prebuilt_dependencies C CXX)

get_filename_component(cmake_custom_pkg_dir "${CMAKE_SOURCE_DIR}/../../cmake"
                       ABSOLUTE)
list(APPEND CMAKE_PREFIX_PATH ${cmake_custom_pkg_dir})
message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

    list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_SOURCE_DIR}/../../cmake")
    message(STATUS "CMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}")
endif()

find_package(om-triplet-name CONFIG REQUIRED)

get_filename_component(prebuilt_dir "${CMAKE_SOURCE_DIR}/../prebuilt" ABSOLUTE)
message(STATUS "${prebuilt_dir}")

set(CMAKE_INSTALL_PREFIX "${prebuilt_dir}/${OM_TRIPLET_NAME}")

if(CMAKE_SYSTEM_NAME EQUAL "Android")
    set(OM_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
elseif(UNIX)
    set(OM_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib") # old lib64
else()
    set(OM_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()

message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")

include(ExternalProject)

externalproject_add(
    hiredis
    GIT_REPOSITORY https://github.com/redis/hiredis.git
    GIT_TAG v1.2.0
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        $<$<PLATFORM_ID:Android>:-DANDROID_ABI=${ANDROID_ABI}>
        $<$<PLATFORM_ID:Android>:-DANDROID_PLATFORM=${ANDROID_PLATFORM}>
        $<$<PLATFORM_ID:Android>:-DCMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}>
        $<$<PLATFORM_ID:Android>:-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}>
        -DBUILD_SHARED_LIBS=OFF
        -DENABLE_SSL=OFF
        -DDISABLE_TESTS=ON
        -DENABLE_SSL_TESTS=OFF
        -DENABLE_ASYNC_TESTS=OFF
        -DENABLE_EXAMPLES=OFF)

externalproject_add(
    redis_plus_plus
    GIT_REPOSITORY https://github.com/sewenew/redis-plus-plus.git
    GIT_TAG 1.3.12
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    DEPENDS hiredis
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        $<$<PLATFORM_ID:Android>:-DANDROID_ABI=${ANDROID_ABI}>
        $<$<PLATFORM_ID:Android>:-DANDROID_PLATFORM=${ANDROID_PLATFORM}>
        $<$<PLATFORM_ID:Android>:-DCMAKE_ANDROID_ARCH_ABI=${CMAKE_ANDROID_ARCH_ABI}>
        $<$<PLATFORM_ID:Android>:-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}>
        $<$<PLATFORM_ID:Android>:-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH>
        -DREDIS_PLUS_PLUS_BUILD_SHARED=OFF
        -DREDIS_PLUS_PLUS_BUILD_STATIC=ON
        -DREDIS_PLUS_PLUS_BUILD_STATIC_WITH_PIC=ON
        -DREDIS_PLUS_PLUS_BUILD_TEST=OFF
        -DREDIS_PLUS_PLUS_BUILD_ASYNC_TEST=OFF
        -DREDIS_PLUS_PLUS_BUILD_MOCK=OFF)

externalproject_add(
    microsoft_gsl
    GIT_REPOSITORY https://github.com/microsoft/GSL.git
    GIT_TAG v4.2.1
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
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
               -DGSL_CXX_STANDARD:STRING=c++23
               -DGSL_INSTALL=ON
               -DGSL_TEST:STRING=OFF)

# Это заставит внешний проект установиться СРАЗУ во время сборки (build time)
# в общую папку установки.
externalproject_add(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
    # Передаем префикс установки вашего основного проекта во внешний
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
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
               -DCATCH_ENABLE_REPRODUCIBLE_BUILD=OFF)
# install SDL3 dependencies from:
# https://wiki.libsdl.org/SDL3/README-linux#build-dependencies
externalproject_add(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.28
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
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
        -DSDL_INSTALL_DOCS=OFF)
externalproject_add(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
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
externalproject_add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
               -DCMAKE_BUILD_TYPE=RelWithDebInfo
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_CXX_STANDARD=23
               -DCMAKE_CXX_STANDARD_REQUIRED=ON
               -DCMAKE_CXX_EXTENSIONS=OFF
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
               -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
               -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS})
externalproject_add(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
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
               -DDOCTEST_USE_STD_HEADERS=ON)
externalproject_add(
    trompeloeil
    GIT_REPOSITORY https://github.com/rollbear/trompeloeil.git
    GIT_TAG v49
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
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
               -DTROMPELOEIL_INSTALL_DOCS=OFF)

externalproject_add(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
               -DCMAKE_BUILD_TYPE=RelWithDebInfo
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
               -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
               -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
               -DINSTALL_LIB_DIR=${OM_INSTALL_LIB_DIR}
               -DZLIB_BUILD_EXAMPLES=OFF
               -DBUILD_SHARED_LIBS=OFF)

externalproject_add(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.91.0
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    DEPENDS zlib
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
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
        -DBUILD_TESTING=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DBOOST_IOSTREAMS_ENABLE_ZLIB=OFF
        -DBOOST_IOSTREAMS_ENABLE_BZIP2=OFF
        -DBOOST_IOSTREAMS_ENABLE_LZMA=OFF
        -DBOOST_IOSTREAMS_ENABLE_ZSTD=OFF)

externalproject_add(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    # fix clang 23 + libc++ build: poly2tri uses std::ostream
    # without including <ostream>
    PATCH_COMMAND
        ${CMAKE_COMMAND}
        -DPATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/assimp-poly2tri-ostream.patch
        -P ${CMAKE_CURRENT_SOURCE_DIR}/patches/git-apply-or-skip.cmake
    DEPENDS zlib
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
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
               -DASSIMP_WARNINGS_AS_ERRORS=OFF)

externalproject_add(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader.git
    GIT_TAG v2.0.0rc13
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
               -DCMAKE_BUILD_TYPE=RelWithDebInfo
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_CXX_STANDARD=23
               -DCMAKE_CXX_STANDARD_REQUIRED=ON
               -DCMAKE_CXX_EXTENSIONS=OFF
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
               -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
               -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
               -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
               -DCMAKE_POLICY_VERSION_MINIMUM=3.16
               -DBUILD_SHARED_LIBS=OFF
               -DTINYOBJLOADER_USE_DOUBLE=OFF
               -DTINYOBJLOADER_WITH_PYTHON=OFF
               -DTINYOBJLOADER_BUILD_TEST_LOADER=OFF
               -DTINYOBJLOADER_BUILD_OBJ_STICHER=OFF)

# stb header-only libraries (installs headers into include/stb)
# https://github.com/nothings/stb
set(stb_src ${CMAKE_BINARY_DIR}/stb-prefix/src/stb)
set(stb_include_dir ${CMAKE_INSTALL_PREFIX}/include/stb)
externalproject_add(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    # only master branch exist no tags to specify
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${stb_include_dir}
    COMMAND ${CMAKE_COMMAND} -E copy ${stb_src}/stb_image.h ${stb_include_dir}/
    COMMAND
        ${CMAKE_COMMAND} -E copy ${stb_src}/stb_image_write.h
        ${stb_include_dir}/
    COMMAND
        ${CMAKE_COMMAND} -E copy ${stb_src}/stb_rect_pack.h ${stb_include_dir}/
    COMMAND
        ${CMAKE_COMMAND} -E copy ${stb_src}/stb_textedit.h ${stb_include_dir}/
    COMMAND
        ${CMAKE_COMMAND} -E copy ${stb_src}/stb_truetype.h ${stb_include_dir}/)

externalproject_add(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.37.1
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
               -DCMAKE_BUILD_TYPE=RelWithDebInfo
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_CXX_STANDARD=23
               -DCMAKE_CXX_STANDARD_REQUIRED=ON
               -DCMAKE_CXX_EXTENSIONS=OFF
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
               -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
               -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
               -DHTTPLIB_COMPILE=OFF
               -DHTTPLIB_TEST=OFF)

externalproject_add(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
               -DCMAKE_BUILD_TYPE=RelWithDebInfo
               -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
               -DCMAKE_CXX_STANDARD=23
               -DCMAKE_CXX_STANDARD_REQUIRED=ON
               -DCMAKE_CXX_EXTENSIONS=OFF
               -DCMAKE_POSITION_INDEPENDENT_CODE=ON
               -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
               -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
               -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
               -DJSON_BuildTests=OFF)

# fastgltf 0.9.1 - glTF 2.0 parser (for 18-vk-gltf-ktx2 example)
# Downloads simdjson singleheader itself at configure time.
externalproject_add(
    fastgltf
    GIT_REPOSITORY https://github.com/spnda/fastgltf.git
    GIT_TAG v0.9.1
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
               -DCMAKE_INSTALL_LIBDIR=lib
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
               -DFASTGLTF_USE_CUSTOM_SMALLVECTOR=OFF
               -DFASTGLTF_ENABLE_TESTS=OFF
               -DFASTGLTF_ENABLE_EXAMPLES=OFF
               -DFASTGLTF_ENABLE_DOCS=OFF
               -DFASTGLTF_ENABLE_GLTF_RS=OFF
               -DFASTGLTF_ENABLE_ASSIMP=OFF
               -DFASTGLTF_DISABLE_CUSTOM_MEMORY_POOL=OFF
               -DFASTGLTF_USE_64BIT_FLOAT=OFF
               -DFASTGLTF_COMPILE_AS_CPP20=OFF
               -DFASTGLTF_ENABLE_CPP_MODULES=ON
               -DFASTGLTF_USE_STD_MODULE=OFF
               -DFASTGLTF_ENABLE_KHR_IMPLICIT_SHAPES=OFF
               -DFASTGLTF_ENABLE_KHR_PHYSICS_RIGID_BODIES=OFF
               -DFASTGLTF_ENABLE_INSTALL=ON)

# KTX-Software 5.0.0-rc2 - KTX2 texture read/write (for 18-vk-gltf-ktx2)
# external/ deps (basis_universal, astc-encoder, dfdutils, ...) are vendored
# in the repo; the only git submodule is tests/cts - skip it.
# Consumers use find_package(KTX CONFIG) -> KTX::ktx
externalproject_add(
    ktx_software
    GIT_REPOSITORY https://github.com/KhronosGroup/KTX-Software.git
    GIT_TAG v5.0.0-rc2
    GIT_PROGRESS ON
    GIT_SHALLOW ON
    GIT_SUBMODULES ""
    # fix clang 23 + libc++ build: basisu uses std::terminate
    # without including <exception>
    PATCH_COMMAND
        ${CMAKE_COMMAND}
        -DPATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/patches/ktx-basisu-exception.patch
        -P ${CMAKE_CURRENT_SOURCE_DIR}/patches/git-apply-or-skip.cmake
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_INSTALL_LIBDIR=lib
        -DCMAKE_BUILD_TYPE=RelWithDebInfo
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}\ -Wno-deprecated-this-capture
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_CXX_STANDARD=23
        -DCMAKE_CXX_STANDARD_REQUIRED=ON
        -DCMAKE_CXX_EXTENSIONS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_LINKER_TYPE=${CMAKE_LINKER_TYPE}
        -DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        -DBUILD_SHARED_LIBS=OFF
        -DKTX_FEATURE_DOC=OFF
        -DKTX_FEATURE_JNI=OFF
        -DKTX_FEATURE_PY=OFF
        -DKTX_FEATURE_TESTS=OFF
        -DKTX_FEATURE_TOOLS_CTS=OFF
        -DKTX_FEATURE_JS=OFF
        -DKTX_FEATURE_TOOLS=ON
        -DKTX_FEATURE_LOADTEST_APPS=OFF
        -DKTX_GENERATE_VK_FILES=OFF
        -DKTX_WERROR=OFF
        -DLIBKTX_VERSION_READ_ONLY=ON
        -DLIBKTX_FEATURE_KTX1=ON
        -DLIBKTX_FEATURE_KTX2=ON
        -DLIBKTX_FEATURE_VK_UPLOAD=ON
        -DLIBKTX_FEATURE_GL_UPLOAD=ON
        -DLIBKTX_FEATURE_ETC_UNPACK=ON)

# upstream installs lib/cmake/ktx/KtxConfig.cmake which find_package() cannot
# find on case-sensitive filesystems (it looks for ktx-config.cmake) -
# install a copy with the expected name
externalproject_add_step(
    ktx_software install_config_compat
    COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_INSTALL_PREFIX}/lib/cmake/ktx/KtxConfig.cmake
        ${CMAKE_INSTALL_PREFIX}/lib/cmake/ktx/ktx-config.cmake
    DEPENDEES install)
