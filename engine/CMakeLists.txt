cmake_minimum_required(VERSION 3.9)
project(engine LANGUAGES CXX)
add_library(om STATIC src/om/om.cxx include/om/om.hxx)
target_include_directories(om PRIVATE include)
target_compile_features(om PRIVATE cxx_std_17)
install(TARGETS om
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
        
