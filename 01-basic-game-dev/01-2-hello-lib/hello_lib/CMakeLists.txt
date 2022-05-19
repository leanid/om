cmake_minimum_required(VERSION 3.22)
project(hello-lib CXX)

# select SHARED or STATIC library
add_library(hello-lib-static STATIC include/hello_lib.hxx src/hello_lib.cxx)
target_include_directories(hello-lib-static
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(hello-lib-static PUBLIC cxx_std_20)

add_library(hello-lib-dynamic SHARED include/hello_lib.hxx src/hello_lib.cxx)
target_include_directories(hello-lib-dynamic
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(hello-lib-dynamic PUBLIC cxx_std_20)

install(TARGETS hello-lib-static hello-lib-dynamic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests)
