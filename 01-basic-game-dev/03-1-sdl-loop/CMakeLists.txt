cmake_minimum_required(VERSION 3.16)
project(03-sdl-loop)

add_executable(03-sdl-loop main.cxx)
target_compile_features(03-sdl-loop PUBLIC cxx_std_17)

find_package(SDL2 REQUIRED)

target_link_libraries(03-sdl-loop PRIVATE SDL2::SDL2 SDL2::SDL2main)


install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
