cmake_minimum_required(VERSION 3.9)
project(hello-lib)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

# select SHARED or STATIC library
add_library(hello-lib STATIC include/hello_lib.hxx src/hello_lib.cxx)
target_include_directories(hello-lib PUBLIC 
                          ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(hello-lib PUBLIC cxx_std_17)
