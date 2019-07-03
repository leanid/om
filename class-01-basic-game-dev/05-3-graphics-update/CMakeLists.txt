cmake_minimum_required(VERSION 3.9)
project(05-3-texture-loading)

add_library(engine-05-3 SHARED engine.cxx)
target_compile_features(engine-05-3 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-05-3 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(sdl2 REQUIRED)
target_include_directories(engine-05-3 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-05-3 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(engine-05-3 PRIVATE
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(APPLE)
   target_link_libraries(engine-05-3 PRIVATE
           -Wl,-framework,OpenGL
           )
elseif(UNIX)
    target_link_libraries(engine-05-3 PRIVATE
               -lSDL2
               -lGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-05-3 PRIVATE SDL2::SDL2 SDL2::SDL2main
                         opengl32)
endif()

add_executable(game-05-3 game.cxx)
target_compile_features(game-05-3 PUBLIC cxx_std_17)

target_link_libraries(game-05-3 PRIVATE engine-05-3)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS engine-05-3 game-05-3
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

