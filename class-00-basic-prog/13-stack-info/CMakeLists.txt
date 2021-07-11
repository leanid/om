cmake_minimum_required(VERSION 3.16)

project(13-stack-info CXX)

add_executable(13-stack-info main.cxx stack_info.hxx stack_info.cxx)
target_compile_features(13-stack-info PRIVATE cxx_std_17)





