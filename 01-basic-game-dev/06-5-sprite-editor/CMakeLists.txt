cmake_minimum_required(VERSION 3.16)
project(06-5-sprite-editor)

add_library(engine-06-5 SHARED engine.cxx
                               engine.hxx
                               picopng.hxx
                               catch.hpp
                               stb_image.h)
target_compile_features(engine-06-5 PUBLIC cxx_std_20)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
target_link_libraries(engine-06-5 PUBLIC imgui_editor)

if(WIN32)
  target_compile_definitions(engine-06-5 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

target_include_directories(engine-06-5 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(engine-06-5 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(engine-06-5 PRIVATE
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
    target_link_libraries(engine-06-5 PRIVATE
      -Wl,-framework,OpenGL
      )
elseif(UNIX)
    target_link_libraries(engine-06-5 PRIVATE
               SDL2::SDL2
               SDL2::SDL2main
               -lGL
               )
elseif(MSVC)
    target_link_libraries(engine-06-5 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_executable(06-5-sprite-editor sprite_editor.cxx
                                  sprite.cxx
                                  sprite_reader.cxx
                                  ani2d.cxx
                                  ani2d.hxx
                                  sprite.hxx
                                  sprite_reader.hxx
                                  )
target_compile_features(06-5-sprite-editor PUBLIC cxx_std_20)
target_link_libraries(06-5-sprite-editor PRIVATE engine-06-5)
add_executable(06-5-test-spr-loader sprite_reader.cxx
                                    sprite.cxx
                                    sprite_reader_test.cxx
                                    sprite_reader.hxx)
target_compile_features(06-5-test-spr-loader PUBLIC cxx_std_20)
target_link_libraries(06-5-test-spr-loader PRIVATE engine-06-5)

install(TARGETS engine-06-5 06-5-sprite-editor
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

