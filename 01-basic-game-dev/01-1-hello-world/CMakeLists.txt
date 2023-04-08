cmake_minimum_required(VERSION 3.22)
project(01-1-hello-world CXX)

add_executable(01-1-hello-world main.cxx)
target_compile_features(01-1-hello-world PUBLIC cxx_std_20)

include(CTest)

add_test(NAME check_hello_exit_zero COMMAND 01-1-hello-world)
