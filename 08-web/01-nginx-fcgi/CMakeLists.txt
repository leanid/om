cmake_minimum_required(VERSION 3.28)

project(fcgi_example CXX)

if(NOT (UNIX OR WIN32))
    message(STATUS "Unsupported platform for NGINX need UNIX or Windows")
    return()
endif()

add_executable(01-nginx-fcgi fcgi_main.cxx)

target_compile_features(01-nginx-fcgi PRIVATE cxx_std_23)
