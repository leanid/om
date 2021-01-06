cmake_minimum_required(VERSION 3.16)
project(03-sdl-loop-to-engine-dll)

add_executable(engine-03-4 engine.cxx engine.hxx)
target_compile_features(engine-03-4 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-03-4 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

set_target_properties(engine-03-4 PROPERTIES
  ENABLE_EXPORTS 1 # we need to be able to link to exe functions from game
  )

find_package(SDL2 REQUIRED)
if(MINGW)
    target_link_libraries(engine-03-4 PRIVATE -lmingw32)
endif()
target_link_libraries(engine-03-4 PRIVATE SDL2::SDL2main SDL2::SDL2)

add_library(game-03-4 SHARED game.cxx)

if(WIN32)
  target_compile_definitions(game-03-4 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

target_compile_features(game-03-4 PUBLIC cxx_std_17)
target_link_libraries(game-03-4 PRIVATE engine-03-4)
