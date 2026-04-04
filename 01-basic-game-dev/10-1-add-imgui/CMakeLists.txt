cmake_minimum_required(VERSION 3.25)

project(10-1-add-imgui CXX)

add_executable(
    engine-10-1
    om/engine.cxx
    om/engine.hxx
    om/gles20.hxx
    om/math.hxx
    om/picopng.hxx)
set_target_properties(engine-10-1 PROPERTIES ENABLE_EXPORTS ON)
target_compile_features(engine-10-1 PRIVATE cxx_std_20)

target_include_directories(engine-10-1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/om)

target_compile_definitions(
    engine-10-1
    PRIVATE "$<$<PLATFORM_ID:Windows>:OM_DECLSPEC=__declspec(dllexport)>")
target_compile_definitions(
    engine-10-1 PRIVATE "$<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS=1>")

target_compile_definitions(
    engine-10-1
    PRIVATE "$<$<PLATFORM_ID:Windows>:IMGUI_API=__declspec(dllexport)>")

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-10-1 PUBLIC om_imgui PRIVATE SDL3::SDL3 OpenGL::GL)

# cmake-format: off
add_library(
    game-10-1 SHARED MODULE
    game/configuration_loader.hxx
    game/game.cxx
    game/game_object.hxx
    )
# cmake-format: on

target_include_directories(game-10-1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(game-10-1 PRIVATE cxx_std_20)

target_link_libraries(game-10-1 PRIVATE engine-10-1)

target_compile_definitions(
    game-10-1
    PRIVATE "$<$<PLATFORM_ID:Windows>:OM_DECLSPEC=__declspec(dllimport)>")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND APPLE)
    # using regular Clang or AppleClang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
