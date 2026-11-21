cmake_minimum_required(VERSION 4.5.1)

project(32-utf8 CXX)

add_executable(32-utf8 main.cxx)
target_compile_features(32-utf8 PRIVATE cxx_std_23)
