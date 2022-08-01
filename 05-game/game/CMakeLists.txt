cmake_minimum_required(VERSION 3.9...3.23)
project(game LANGUAGES CXX)

if(MSVC)
  #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_library(game SHARED game.cxx)

target_compile_features(game PRIVATE cxx_std_20)
target_link_libraries(game PRIVATE om)
add_custom_command(
  TARGET game
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:game>" "$<TARGET_FILE_DIR:om>"
)
