cmake_minimum_required(VERSION 3.16..3.22)
project(04-2-opengl-triangle)

add_library(engine-04-2 SHARED engine.cxx engine.hxx)
target_compile_features(engine-04-2 PUBLIC cxx_std_17)

if(WIN32)
    target_compile_definitions(engine-04-2
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-04-2 PRIVATE SDL2::SDL2 SDL2::SDL2main OpenGL::GL)

add_executable(game-04-2 game.cxx)
target_compile_features(game-04-2 PUBLIC cxx_std_17)
target_link_libraries(game-04-2 PRIVATE engine-04-2)

install(TARGETS engine-04-2 game-04-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
