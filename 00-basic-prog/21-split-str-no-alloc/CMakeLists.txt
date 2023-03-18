cmake_minimum_required(VERSION 3.14...3.25)
project(21-split-str-no-alloc)
add_executable(${PROJECT_NAME} main.cxx)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
