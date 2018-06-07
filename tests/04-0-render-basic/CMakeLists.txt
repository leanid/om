cmake_minimum_required(VERSION 3.9)
project(04-0-render-basic)

add_executable(04-0-render-basic graphics_basic.cxx
                                 graphics_basic.hxx)
target_compile_features(04-0-render-basic PUBLIC cxx_std_17)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS 04-0-render-basic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
