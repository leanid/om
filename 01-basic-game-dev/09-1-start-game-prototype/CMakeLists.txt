cmake_minimum_required(VERSION 3.19)
project(09-1-start-game-protorype)

add_executable(engine-09-1 om/engine.cxx
                           om/engine.hxx
                           om/picopng.hxx
                           om/math.hxx)
set_target_properties(engine-09-1 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-09-1 PRIVATE cxx_std_20)

if(WIN32)
  target_compile_definitions(engine-09-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

if (MINGW)
    target_link_libraries(engine-09-1
               mingw32
               SDL2main
               SDL2
               -mwindows
               opengl32
               )
elseif(UNIX)
    target_link_libraries(engine-09-1
               SDL2::SDL2
               SDL2::SDL2main
               OpenGL::OpenGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-09-1 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_library(game-09-1 SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx

                        res/level_01.txt
                        res/identity_quad.txt
                        res/background.png
                        res/tank.png
                        )
target_include_directories(game-09-1 PRIVATE .)
target_compile_features(game-09-1 PRIVATE cxx_std_20)
target_link_libraries(game-09-1 PRIVATE engine-09-1)

if(MSVC)
  target_compile_definitions(game-09-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

install(TARGETS engine-09-1 game-09-1
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
