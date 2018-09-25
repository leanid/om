cmake_minimum_required(VERSION 3.12)
project(03-int-type)
add_executable(${PROJECT_NAME} hack_main.cxx)
target_compile_features(${PROJECT_NAME} PRIVATE std_cxx_17)

