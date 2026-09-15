cmake_minimum_required(VERSION 3.22)

project(greedy-lib CXX)

# Одна и та же "чужая" библиотека в двух видах: статическая и динамическая.
add_library(greedy-lib-static STATIC include/greedy_lib.hxx
                                     src/greedy_lib.cxx)
target_include_directories(greedy-lib-static
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(greedy-lib-static PUBLIC cxx_std_20)

add_library(greedy-lib-shared SHARED include/greedy_lib.hxx
                                     src/greedy_lib.cxx)
target_include_directories(greedy-lib-shared
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(greedy-lib-shared PUBLIC cxx_std_20)
