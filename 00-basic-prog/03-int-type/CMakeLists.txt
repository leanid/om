cmake_minimum_required(VERSION 3.14)
project(03-int-type)
add_executable(${PROJECT_NAME} main.cxx bool.hxx bool.cxx)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

