cmake_minimum_required(VERSION 3.9)
project("10-1-integrate-imgui")

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_executable(engine om/engine.cxx om/engine.hxx om/picopng.hxx)
set_target_properties(engine PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine PRIVATE cxx_std_17)

if(WIN32)
  target_compile_definitions(engine PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               -lstdc++fs
               )
elseif(UNIX)
    target_link_libraries(engine
               -lSDL2
               -lGL
               -lstdc++fs
               )
endif()

add_library(game SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx)
target_include_directories(game PRIVATE .)
target_compile_features(game PRIVATE cxx_std_17)

target_link_libraries(game engine)
