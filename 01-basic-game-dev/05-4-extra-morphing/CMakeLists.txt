cmake_minimum_required(VERSION 3.17)
project(05-4-extra-morphing)

add_library(engine-05-4 SHARED engine.cxx engine.hxx picopng.hxx)
target_compile_features(engine-05-4 PUBLIC cxx_std_20)

if(WIN32)
    target_compile_definitions(engine-05-4
                               PRIVATE "-DOM_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(engine-05-4 PRIVATE SDL3::SDL3-shared OpenGL::GL)

add_executable(game-05-4 game.cxx loader_obj.cxx loader_obj.hxx)
target_compile_features(game-05-4 PUBLIC cxx_std_20)
target_link_libraries(game-05-4 PRIVATE engine-05-4)

set(RESOURCE_FILES
    tank.png
    circle.mtl
    circle.obj
    quad.mtl
    quad.obj
    vert_pos.txt
    vert_pos_color.txt
    vert_tex_color.txt)

set_target_properties(
    game-05-4
    PROPERTIES MACOSX_BUNDLE TRUE MACOSX_FRAMEWORK_IDENTIFIER om.game-05-4
               RESOURCE "${RESOURCE_FILES}")
