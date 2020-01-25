cmake_minimum_required(VERSION 3.16)

project(08-aslr CXX)
add_executable(08-aslr "main.cxx")
target_compile_features(08-aslr PRIVATE cxx_std_17)

add_executable(08-aslr-test "check_aslr_repeat.cxx")
target_compile_features(08-aslr-test PRIVATE cxx_std_17)

add_library(08-exec-stream STATIC exec-stream.h exec-stream.cpp)

if(UNIX)
    target_sources(08-aslr-test INTERFACE
        posix/exec-stream-helpers.cpp
        posix/exec-stream-helpers.h
        posix/exec-stream-impl.cpp
        )
    target_link_libraries(08-aslr-test PRIVATE pthread)
else()
    target_sources(08-aslr-test INTERFACE
        win/exec-stream-helpers.cpp
        win/exec-stream-helpers.h
        win/exec-stream-impl.cpp
        )
endif()

target_link_libraries(08-aslr-test PRIVATE 08-exec-stream)



