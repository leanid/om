cmake_minimum_required(VERSION 3.22...3.29)

project(01-nginx-fcgi CXX)

if(NOT UNIX)
    message(STATUS "Unsupported platform for NGINX need UNIX")
    return()
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(01-nginx-fcgi fcgi_main.cxx)
# sudo apt install libfcgi-dev (on Ubuntu)
# sudo dnf install fcgi-devel (on Fedora)
find_library(FCGI fcgi REQUIRED)

target_link_libraries(01-nginx-fcgi PRIVATE ${FCGI})
