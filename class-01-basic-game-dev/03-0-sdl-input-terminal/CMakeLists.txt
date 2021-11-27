cmake_minimum_required(VERSION 3.20)
project(03-0-sdl-input-terminal)

add_executable(03-0-sdl-input-terminal main.cxx)
target_compile_features(03-0-sdl-input-terminal PUBLIC cxx_std_20)

find_package(SDL2 REQUIRED)

target_link_libraries(03-0-sdl-input-terminal PRIVATE SDL2::SDL2 SDL2::SDL2main)


install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
  LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
  ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
