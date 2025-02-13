cmake_minimum_required(VERSION 3.31)

project(19-boost-file-system CXX)

add_executable(main_boost main.cxx)

find_package(boost_filesystem CONFIG REQUIRED)

target_link_libraries(main_boost PRIVATE Boost::filesystem)
