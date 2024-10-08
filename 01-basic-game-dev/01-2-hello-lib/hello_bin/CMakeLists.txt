cmake_minimum_required(VERSION 3.22)
project(hello-bin CXX)

if(NOT MINGW AND NOT MSVC AND NOT APPLE)
    message(STATUS "try to fix MinGW64 static linking error for F31 MinGW 6.0")

    add_executable(hello-bin-static src/main.cxx)
    target_compile_features(hello-bin-static PRIVATE cxx_std_20)
    target_link_libraries(hello-bin-static PUBLIC hello-lib-static)
    # Fedora `sudo dnf isntall libcxx-devel libcxx-static` for clang++
    target_link_options(hello-bin-static PRIVATE -static)
endif()

add_compile_definitions($<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS=1>)

add_executable(hello-bin-dynamic src/main.cxx)
if(MSVC)
    target_compile_definitions(hello-bin-dynamic
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
endif(MSVC)
target_compile_features(hello-bin-dynamic PRIVATE cxx_std_20)
target_link_libraries(hello-bin-dynamic PUBLIC hello-lib-dynamic)
