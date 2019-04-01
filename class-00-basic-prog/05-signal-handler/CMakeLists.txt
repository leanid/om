cmake_minimum_required(VERSION 3.14)

project(05-signal-handler)
add_executable(05-signal-handler "main.cpp")
target_compile_features(05-signal-handler PRIVATE cxx_std_17)

add_executable(05-signal-handler-throw "throw_from_sig_handler.cpp")
target_compile_features(05-signal-handler-throw PRIVATE cxx_std_17)
