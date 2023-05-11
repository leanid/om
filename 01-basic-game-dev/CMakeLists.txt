cmake_minimum_required(VERSION 3.16)

project(01-basic-game-dev CXX)

add_subdirectory(01-1-hello-world)
add_subdirectory(01-2-hello-lib)

add_subdirectory(02-1-sdl-dynamic)
add_subdirectory(02-2-sdl-static)

add_subdirectory(03-0-sdl-input-terminal)
add_subdirectory(03-1-sdl-loop)
add_subdirectory(03-2-sdl-loop-to-engine)
add_subdirectory(03-3-sdl-loop-to-engine-dll)
add_subdirectory(03-4-sdl-loop-hot-reload)

add_subdirectory(04-0-render-basic)
add_subdirectory(04-1-opengl-minimal)
add_subdirectory(04-2-opengl-triangle)
add_subdirectory(04-3-opengl-renderdoc-check)

add_subdirectory(05-1-vertex-morphing)
add_subdirectory(05-2-texture-loading)
add_subdirectory(05-3-graphics-update)
add_subdirectory(05-4-extra-morphing)

add_subdirectory(06-1-matrix-and-vector)
add_subdirectory(06-2-shader-multiply-matrix)
add_subdirectory(06-3-move-tank)
add_subdirectory(06-4-add-imgui)
add_subdirectory(06-5-sprite-editor)

add_subdirectory(07-1-sdl-sound-example)
add_subdirectory(07-2-add-sound-to-engine)

add_subdirectory(08-1-prepare-engine-for-game)
add_subdirectory(08-2-engine-load-game-from-dll)
add_subdirectory(08-3-engine-cleanup)

add_subdirectory(09-1-start-game-prototype)
add_subdirectory(09-2-snake-game)
add_subdirectory(09-3-snake-game-android)

add_subdirectory(10-1-add-imgui)
