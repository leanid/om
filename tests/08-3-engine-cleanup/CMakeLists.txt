cmake_minimum_required(VERSION 3.9)
project(08-3-engine-cleanup)

add_executable(engine-08-3 engine.cxx engine.hxx picopng.hxx)
set_target_properties(engine-08-3 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-08-3 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-08-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-08-3
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(UNIX)
    target_link_libraries(engine-08-3
               -lSDL2
               -lGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-08-3 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_library(game-08-3 SHARED game.cxx)
target_compile_features(game-08-3 PUBLIC cxx_std_17)

target_link_libraries(game-08-3 engine-08-3)

if(MSVC)
  target_compile_definitions(game-08-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
