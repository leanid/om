cmake_minimum_required(VERSION 3.31)

project(35-catch2 CXX)

add_library(amalgamated_catch2 OBJECT catch_amalgamated.cpp
                                      catch_amalgamated.hpp)
target_include_directories(amalgamated_catch2 PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_library(fakeit INTERFACE)
target_include_directories(fakeit INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_executable(35-catch2 main.cxx)
target_link_libraries(35-catch2 PRIVATE amalgamated_catch2)

add_executable(35-catch2-fakeit main_moc.cxx)
target_link_libraries(35-catch2-fakeit PRIVATE amalgamated_catch2 fakeit)

enable_testing()

add_test(NAME 35-catch2 COMMAND 35-catch2)
add_test(NAME 35-catch2-fakeit COMMAND 35-catch2-fakeit)
