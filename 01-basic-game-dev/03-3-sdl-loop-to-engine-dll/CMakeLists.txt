cmake_minimum_required(VERSION 3.16)
project(03-sdl-loop-to-engine-dll)

add_library(engine-03-3 SHARED engine.cxx engine.hxx)
target_compile_features(engine-03-3 PUBLIC cxx_std_20)

if(WIN32)   
  target_compile_definitions(engine-03-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)

target_link_libraries(engine-03-3 PRIVATE SDL2::SDL2 SDL2::SDL2main)

add_executable(game-03-3 game.cxx)
target_compile_features(game-03-3 PUBLIC cxx_std_20)

target_link_libraries(game-03-3 PRIVATE engine-03-3)

install(TARGETS engine-03-3 game-03-3
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
