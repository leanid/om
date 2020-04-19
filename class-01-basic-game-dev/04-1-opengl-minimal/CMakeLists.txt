cmake_minimum_required(VERSION 3.16)
project(04-1-opengl-minimal CXX C)

add_library(engine-04-1 SHARED
    engine.cxx
    engine.hxx
    KHR/khrplatform.h
    glad/glad.h
    glad/glad.c)
target_compile_features(engine-04-1 PUBLIC cxx_std_17)
target_include_directories(engine-04-1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_compile_definitions(engine-04-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

# use sdl >= 2.0.12 (with nice new cmake build file)
find_package(sdl2 REQUIRED)
target_link_libraries(engine-04-1 PRIVATE SDL2::SDL2 SDL2::SDL2main)

if (MINGW)
    target_link_libraries(engine-04-1 PRIVATE
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif (APPLE)
    target_link_libraries(engine-04-1 PRIVATE -Wl,-framework,OpenGL)
elseif (UNIX)
    target_link_libraries(engine-04-1 PRIVATE GL)
elseif(MSVC)
    target_link_libraries(engine-04-1 PRIVATE opengl32)
endif()

add_executable(game-04-1 game.cxx)
target_compile_features(game-04-1 PUBLIC cxx_std_17)
target_link_libraries(game-04-1 PRIVATE engine-04-1)

install(TARGETS engine-04-1 game-04-1
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

