cmake_minimum_required(VERSION 3.16)

project(07-multi-threading CXX)
add_executable(07-multi-threading "main.cxx")
target_compile_features(07-multi-threading PRIVATE cxx_std_17)

if(UNIX)
    target_link_libraries(07-multi-threading PRIVATE pthread)
endif()
