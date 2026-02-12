cmake_minimum_required(VERSION 3.16)
project(06-5-sprite-editor)

add_library(
    engine-06-5 SHARED
    catch.hpp
    engine.cxx
    engine.hxx
    picopng.hxx)
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
target_link_libraries(engine-06-5 PRIVATE stb::stb)

add_executable(
    06-5-sprite-editor
    ani2d.cxx
    ani2d.hxx
    sprite.cxx
    sprite.hxx
    sprite_editor.cxx
        sprite_io.cxx
        sprite_io.hxx)
target_compile_features(06-5-sprite-editor PUBLIC cxx_std_20)
target_link_libraries(06-5-sprite-editor PRIVATE engine-06-5)
add_executable(
    06-5-test-spr-loader
    sprite.cxx
        sprite_io.cxx
        sprite_io.hxx
        sprite_io_test.cxx)
target_compile_features(06-5-test-spr-loader PUBLIC cxx_std_20)
target_link_libraries(06-5-test-spr-loader PRIVATE engine-06-5)

option(OM_ENABLE_ADDRESS_SANITIZER_FOR_SPRITE_TEST
       [[Use address sanitizer to find out memory bug.
Works only on Linux. You should already install library:
```sudo dnf install libasan```
To see line numbers in asan log configure your build with
```CMAKE_BUILD_TYPE=Debug```.]] OFF)

if(OM_ENABLE_ADDRESS_SANITIZER_FOR_SPRITE_TEST)
    message(
        STATUS
            "use address sanitizer to find memory bug in 06-5-test-spr-loader")
    add_library(sanitizer INTERFACE)
    target_compile_options(sanitizer INTERFACE -fsanitize=address)
    target_link_options(sanitizer INTERFACE -fsanitize=address)

    target_link_libraries(engine-06-5 PRIVATE sanitizer)
    target_link_libraries(06-5-test-spr-loader PRIVATE sanitizer)
endif()
