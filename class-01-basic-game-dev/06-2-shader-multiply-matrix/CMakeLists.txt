cmake_minimum_required(VERSION 3.16)
project(06-2-shader-multiply-matrix CXX)

add_library(engine-06-2 SHARED engine.cxx
                               engine.hxx
                               picopng.hxx)
target_compile_features(engine-06-2 PUBLIC cxx_std_17)

if(WIN32)   
  target_compile_definitions(engine-06-2 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(sdl2 REQUIRED)
target_include_directories(engine-06-2 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-06-2 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(engine-06-2
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(APPLE)
    target_link_libraries(engine-06-2 PRIVATE
         -Wl,-framework,OpenGL
         )
elseif(UNIX)
    target_link_libraries(engine-06-2 PRIVATE
               -lSDL2
               -lGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-06-2 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_executable(game-06-2 game.cxx)
target_compile_features(game-06-2 PUBLIC cxx_std_17)
target_link_libraries(game-06-2 PRIVATE engine-06-2)

install(TARGETS engine-06-2 game-06-2
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

