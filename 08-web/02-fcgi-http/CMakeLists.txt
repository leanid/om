cmake_minimum_required(VERSION 3.22...3.29)

project(02-fcgi-http CXX)

if(NOT UNIX)
    message(STATUS "Unsupported platform for NGINX need UNIX")
    return()
endif()

include(CheckCXXSymbolExists)

check_cxx_symbol_exists(__cpp_lib_jthread "thread" cxx_have_jthread)

if(NOT cxx_have_jthread)
    message(STATUS "skip 02-fcgi-http no std::jthread")
    return()
endif()
# sudo apt install libfcgi-dev (on Ubuntu)
# sudo dnf install fcgi-devel (on Fedora)
find_library(FCGI fcgi)

if(NOT FCGI)
    message(STATUS "skip 02-fcgi-http no lib FastCGI")
    return()
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(02-fcgi-http 02_fcgi_http.cxx)

target_link_libraries(02-fcgi-http PRIVATE ${FCGI})
