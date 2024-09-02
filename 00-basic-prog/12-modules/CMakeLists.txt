cmake_minimum_required(VERSION 3.30)
# implement it when cmake+ninja+g++ support modules out of the box

set(CMAKE_CXX_EXTENSIONS ON) # needed for cxx modules

add_executable(modules-12 hello.cxx main.cxx)
