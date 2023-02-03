cmake_minimum_required(VERSION 3.16)

project(08-aslr CXX)
add_executable(08-aslr "main.cxx")
target_compile_features(08-aslr PRIVATE cxx_std_20)

add_executable(08-aslr-test "check_aslr_repeat.cxx")
target_compile_features(08-aslr-test PRIVATE cxx_std_20)

add_library(08-exec-stream STATIC exec-stream.h exec-stream.cpp)

target_sources(08-exec-stream PRIVATE
    posix/exec-stream-helpers.hpp
    posix/exec-stream-helpers.h
    posix/exec-stream-impl.hpp
    win/exec-stream-helpers.hpp
    win/exec-stream-helpers.h
    win/exec-stream-impl.hpp
    )

if(UNIX)
    target_link_libraries(08-exec-stream PRIVATE pthread)
endif()

target_link_libraries(08-aslr-test PRIVATE 08-exec-stream)

add_dependencies(08-aslr-test 08-aslr)



