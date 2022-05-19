cmake_minimum_required(VERSION 3.16)
project(10-1-add-imgui CXX)

add_executable(engine-10-1 om/engine.cxx
                           om/imgui_impl_sdl_gl3.cpp
                           om/imgui.cpp
                           om/imgui_draw.cpp
                           om/imgui_demo.cpp
                           om/imgui.h
                           om/imgui_impl_sdl_gl3.h
                           om/imgui_internal.h
                           om/imconfig.h
                           om/gles20.hxx
                           om/engine.hxx
                           om/picopng.hxx
                           om/math.hxx
                           om/stb_rect_pack.h
                           om/stb_textedit.h
                           om/stb_truetype.h)
set_target_properties(engine-10-1 PROPERTIES ENABLE_EXPORTS TRUE)
target_compile_features(engine-10-1 PRIVATE cxx_std_17)

if(WIN32)
  target_compile_definitions(engine-10-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
  target_compile_definitions(engine-10-1 PRIVATE "-DIMGUI_API=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine-10-1
               mingw32
               SDL2main
               SDL2
               -mwindows
               opengl32
               )
elseif(UNIX)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(engine-10-1 SDL2::SDL2
        SDL2::SDL2main
        OpenGL::GL)
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(engine-10-1 PRIVATE SDL2::SDL2 SDL2::SDL2main
                          opengl32)
endif()

add_library(game-10-1 SHARED game/game.cxx 
                        game/configuration_loader.hxx
                        game/game_object.hxx)
target_include_directories(game-10-1 PRIVATE .)
target_compile_features(game-10-1 PRIVATE cxx_std_17)

target_link_libraries(game-10-1 engine-10-1)

if(MSVC)
  target_compile_definitions(game-10-1 PRIVATE "-DOM_DECLSPEC=__declspec(dllimport)")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

install(TARGETS engine-10-1 game-10-1
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
