cmake_minimum_required(VERSION 3.16)
project(04-3-opengl-renderdoc-check)

add_library(engine-04-3 STATIC engine.cxx
                               engine.hxx)
target_compile_features(engine-04-3 PUBLIC cxx_std_20)

if(WIN32)
  target_compile_definitions(engine-04-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)
target_link_libraries(engine-04-3 PRIVATE SDL2::SDL2 SDL2::SDL2main)

if (MINGW)
    target_link_libraries(engine-04-3 PRIVATE
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(APPLE)
   target_link_libraries(engine-04-3 PRIVATE -Wl,-framework,OpenGL)
elseif(UNIX)
    target_link_libraries(engine-04-3 PRIVATE GL)
elseif(MSVC)
    target_link_libraries(engine-04-3 PRIVATE opengl32)
endif()

add_executable(game-04-3 game.cxx)
target_compile_features(game-04-3 PUBLIC cxx_std_20)
target_link_libraries(game-04-3 PRIVATE engine-04-3)

install(TARGETS engine-04-3 game-04-3
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

