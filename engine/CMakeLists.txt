cmake_minimum_required(VERSION 3.9)
project(engine LANGUAGES CXX)

find_library(SDL2_LIB SDL2)

add_library(om SHARED src/om/main.cxx 
                      src/om/engine_impl.cxx
                      include/om/engine.hxx
                      include/om/game.hxx)
                      
target_include_directories(om PRIVATE include)
target_link_libraries(om PRIVATE ${SDL2LIB})
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
        
