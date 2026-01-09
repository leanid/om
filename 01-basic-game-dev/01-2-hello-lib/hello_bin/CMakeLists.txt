cmake_minimum_required(VERSION 3.22)
project(hello-bin CXX)

include(CheckCXXCompilerFlag)
include(CheckLinkerFlag)

if(NOT MINGW AND NOT MSVC AND NOT APPLE)
    check_cxx_compiler_flag("-static" CXX_SUPPORTS_STATIC_LINKING)
    check_linker_flag(CXX "-static" LINKER_SUPPORTS_STATIC_LINKING)
    message(STATUS "status [-static] flag: CXX_SUPPORTS_STATIC_LINKING: ${CXX_SUPPORTS_STATIC_LINKING} LINKER_SUPPORTS_STATIC_LINKING: ${LINKER_SUPPORTS_STATIC_LINKING}")

    if(CXX_SUPPORTS_STATIC_LINKING AND LINKER_SUPPORTS_STATIC_LINKING AND FALSE)
        add_executable(hello-bin-static src/main.cxx)
        target_compile_features(hello-bin-static PRIVATE cxx_std_20)
        target_link_libraries(hello-bin-static PUBLIC hello-lib-static)
        # Fedora `sudo dnf isntall libcxx-devel libcxx-static` for clang++
        # Ubuntu `libc++-dev libc++abi-dev`
        target_link_options(hello-bin-static PRIVATE -static)
    else()
        message(
            STATUS "static linking is not supported or you need to install"
                   " libcxx-devel libcxx-static for clang++"
                   " or you need to install libc++-devel libc++-static for g++")
    endif()
endif()

add_compile_definitions($<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS=1>)

add_executable(hello-bin-dynamic src/main.cxx)
if(MSVC)
    target_compile_definitions(hello-bin-dynamic
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
endif(MSVC)
target_compile_features(hello-bin-dynamic PRIVATE cxx_std_20)
target_link_libraries(hello-bin-dynamic PUBLIC hello-lib-dynamic)
