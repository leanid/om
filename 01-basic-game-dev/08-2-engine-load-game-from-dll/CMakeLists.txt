cmake_minimum_required(VERSION 3.19)
project(08-2-engine-load-game-from-dll)

add_executable(engine-08-2 engine.cxx engine.hxx picopng.hxx)
set_target_properties(engine-08-2 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-08-2 PUBLIC cxx_std_20)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
#target_include_directories(engine-08-2 PRIVATE ${SDL2_INCLUDE_DIRS})
#target_link_libraries(engine-08-2 PRIVATE ${SDL2_LIBRARIES})

if(WIN32)
  target_compile_definitions(engine-08-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
  target_compile_definitions(engine-08-2 PRIVATE "-DOM_GAME=__declspec(dllimport)")  # for om_tat_sat import
endif(WIN32)

if (MINGW)
    target_link_libraries(engine-08-2 PRIVATE
               SDL2::SDL2
               SDL2::SDL2main
               OpenGL::GL
               #-lmingw32
               #-lSDL2main
               #-lSDL2
               #-mwindows
               #-lopengl32
               )
elseif(APPLE)
    target_link_libraries(engine-08-2 PRIVATE
              -Wl,-framework,OpenGL
              )
elseif(MSVC OR UNIX)
  target_link_libraries(engine-08-2 PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    OpenGL::GL)
endif()

add_library(game-08-2 MODULE game.cxx)
target_compile_features(game-08-2 PUBLIC cxx_std_20)

target_link_libraries(game-08-2 PRIVATE engine-08-2)

if(MSVC)
  # for all engine functions
  target_compile_definitions(game-08-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
  # for om_tat_sat export
  target_compile_definitions(game-08-2 PRIVATE "-DOM_GAME=__declspec(dllexport)")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-08-2 game-08-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
