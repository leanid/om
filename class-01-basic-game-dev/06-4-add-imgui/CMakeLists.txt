cmake_minimum_required(VERSION 3.16)
project(06-4-add-imgui)

add_library(engine-06-4 SHARED engine.cxx
                               engine.hxx
                               picopng.hxx)
target_compile_features(engine-06-4 PUBLIC cxx_std_17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
target_link_libraries(engine-06-4 PRIVATE imgui)

if(WIN32)
  target_compile_definitions(engine-06-4 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(sdl2 REQUIRED)
target_include_directories(engine-06-4 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-06-4 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(engine-06-4 PRIVATE
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(APPLE)
   target_link_libraries(engine-06-4 PRIVATE
       -Wl,-framework,OpenGL
       )
elseif(UNIX)
    target_link_libraries(engine-06-4 PRIVATE
               -lSDL2
               -lGL
               )
elseif(MSVC)
    target_link_libraries(engine-06-4 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_executable(game-06-4 game.cxx)
target_compile_features(game-06-4 PUBLIC cxx_std_17)
target_link_libraries(game-06-4 PRIVATE engine-06-4)

install(TARGETS engine-06-4 game-06-4
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

