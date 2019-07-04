cmake_minimum_required(VERSION 3.9)
project(08-1-prepare-engine-for-game)

add_library(engine-08-1 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-08-1 PUBLIC cxx_std_17)

find_package(sdl2 REQUIRED)
target_include_directories(engine-08-1 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-08-1 PRIVATE ${SDL2_LIBRARIES})

if(WIN32)
  target_compile_definitions(engine-08-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-08-1 PRIVATE
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(APPLE)
    target_link_libraries(engine-08-1 PRIVATE
              -Wl,-framework,OpenGL
              )
elseif(UNIX)
    target_link_libraries(engine-08-1 PRIVATE
               -lSDL2
               -lGL
               )
elseif(MSVC)
    target_link_libraries(engine-08-1 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_executable(game-08-1 game.cxx)
target_compile_features(game-08-1 PUBLIC cxx_std_17)

target_link_libraries(game-08-1 PRIVATE engine-08-1)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-08-1 game-08-1
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
