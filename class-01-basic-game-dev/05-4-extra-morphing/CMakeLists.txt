cmake_minimum_required(VERSION 3.17)
project(05-4-extra-morphing)

add_library(engine-05-4 SHARED engine.cxx
                               engine.hxx
                               picopng.hxx)
target_compile_features(engine-05-4 PUBLIC cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-05-4 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(sdl2 REQUIRED)
target_include_directories(engine-05-4 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-05-4 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(engine-05-4 PRIVATE
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(APPLE)
   target_link_libraries(engine-05-4 PRIVATE
           -Wl,-framework,OpenGL
           )
elseif(UNIX)
    target_link_libraries(engine-05-4 PRIVATE
               -lSDL2
               -lGL
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-05-4 PRIVATE SDL2::SDL2 SDL2::SDL2main
                         opengl32)
endif()

add_executable(game-05-4 game.cxx
                         loader_obj.hxx)
target_compile_features(game-05-4 PUBLIC cxx_std_17)
target_link_libraries(game-05-4 PRIVATE engine-05-4)

set(RESOURCE_FILES tank.png
                   circle.mtl
                   circle.obj
                   quad.mtl
                   quad.obj

                   vert_pos.txt
                   vert_pos_color.txt
                   vert_tex_color.txt)

set_target_properties(game-05-4 PROPERTIES
  MACOSX_BUNDLE TRUE
  MACOSX_FRAMEWORK_IDENTIFIER om.game-05-4
  RESOURCE "${RESOURCE_FILES}")

install(TARGETS engine-05-4 game-05-4
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

