cmake_minimum_required(VERSION 3.9)
project(game LANGUAGES CXX)


find_library(OM_LIB om PATHS ${CMAKE_CURRENT_LIST_DIR}/../bin/)

add_library(game SHARED game.cxx)

target_include_directories(game PRIVATE ../engine/include)
target_compile_features(game PRIVATE cxx_std_17)
target_link_libraries(game PRIVATE ${OM_LIB})

install(TARGETS game
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin)
        
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
