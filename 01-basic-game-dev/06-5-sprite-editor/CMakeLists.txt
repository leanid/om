cmake_minimum_required(VERSION 3.16)
project(06-5-sprite-editor)

add_library(
    engine-06-5 SHARED
    catch.hpp
    engine.cxx
    engine.hxx
    picopng.hxx
    stb_image.h)
target_compile_features(engine-06-5 PUBLIC cxx_std_20)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
target_link_libraries(engine-06-5 PUBLIC imgui_editor)

if(WIN32)
    target_compile_definitions(engine-06-5
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-06-5 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_executable(
    06-5-sprite-editor
    ani2d.cxx
    ani2d.hxx
    sprite.cxx
    sprite.hxx
    sprite_editor.cxx
    sprite_reader.cxx
    sprite_reader.hxx)
target_compile_features(06-5-sprite-editor PUBLIC cxx_std_20)
target_link_libraries(06-5-sprite-editor PRIVATE engine-06-5)
add_executable(
    06-5-test-spr-loader
    sprite.cxx
    sprite_reader.cxx
    sprite_reader.hxx
    sprite_reader_test.cxx)
target_compile_features(06-5-test-spr-loader PUBLIC cxx_std_20)
target_link_libraries(06-5-test-spr-loader PRIVATE engine-06-5)
