cmake_minimum_required(VERSION 3.20...3.22)

project(18-stacktrace CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_ENABLE_EXPORTS OFF)

add_executable(main main.cxx)
target_link_libraries(main
                      PRIVATE $<$<CXX_COMPILER_ID:GNU>:stdc++_libbacktrace>)
