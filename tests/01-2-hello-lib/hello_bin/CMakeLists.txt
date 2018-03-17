cmake_minimum_required(VERSION 3.9)
project(hello-bin)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_executable(hello-bin src/main.cxx)
target_compile_features(hello-bin PRIVATE cxx_std_17)
target_link_libraries(hello-bin LINK_PUBLIC hello-lib)
