cmake_minimum_required(VERSION 3.16)
project(04-3-opengl-renderdoc-check)

add_library(engine-04-3 STATIC engine.cxx engine.hxx)
target_compile_features(engine-04-3 PUBLIC cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-04-3
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(engine-04-3 PRIVATE SDL3::SDL3 OpenGL::GL)

add_executable(game-04-3 game.cxx)
target_compile_features(game-04-3 PUBLIC cxx_std_20)
target_link_libraries(game-04-3 PRIVATE engine-04-3)

install(TARGETS engine-04-3 game-04-3
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
