cmake_minimum_required(VERSION 3.16)
project(04-0-render-basic CXX)

add_executable(04-0-render-basic graphics_basic.cxx
                                 graphics_basic.hxx)
target_compile_features(04-0-render-basic PUBLIC cxx_std_17)

install(TARGETS 04-0-render-basic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
