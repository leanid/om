cmake_minimum_required(VERSION 3.9)
project(08-2-engine-load-game-from-dll)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_executable(engine-08-2 engine.cxx engine.hxx picopng.hxx)
set_target_properties(engine-08-2 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-08-2 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-08-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-08-2
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine-08-2
               -lSDL2
               -lGL
               )
endif()

add_library(game-08-2 SHARED game.cxx)
target_compile_features(game-08-2 PUBLIC cxx_std_17)

target_link_libraries(game-08-2 engine-08-2)
