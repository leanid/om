cmake_minimum_required(VERSION 3.31)

project(34-doctest CXX)

add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_executable(34-doctest main.cxx)
target_link_libraries(34-doctest PRIVATE doctest)

enable_testing()

add_test(NAME 34-doctest COMMAND 34-doctest)
