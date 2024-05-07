cmake_minimum_required(VERSION 3.22...3.29)

project(01-nginx-fcgi CXX)

if(NOT UNIX)
    message(STATUS "Unsupported platform for NGINX need UNIX")
    return()
endif()

include(CheckCXXSymbolExists)

check_cxx_symbol_exists(__cpp_lib_jthread "thread" cxx_have_jthread)

if(NOT cxx_have_jthread)
    message(STATUS "skip 01-nginx-fcgi no std::jthread")
    return()
endif()
# sudo apt install libfcgi-dev (on Ubuntu)
# sudo dnf install fcgi-devel (on Fedora)
find_library(FCGI fcgi)

if(NOT FCGI)
    message(STATUS "skip 01-nginx-fcgi no lib FastCGI")
    return()
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(01-nginx-fcgi fcgi_main.cxx)

target_link_libraries(01-nginx-fcgi PRIVATE ${FCGI})
