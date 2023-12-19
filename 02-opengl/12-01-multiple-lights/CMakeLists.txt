cmake_minimum_required(VERSION 3.14.0)

if(SDL2_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL2_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL2_BUILD_DIR}_${ANDROID_ABI} )
endif()

project(12-01-multiple-lights CXX)

if (SDL2_SRC_DIR) # Android
    add_library(${PROJECT_NAME} SHARED)
    find_library(SDL2_LIB NAMES SDL2)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_SRC_DIR}/include )
    target_link_libraries(${PROJECT_NAME} SDL2 SDL2main )
elseif(MINGW)
    add_executable(${PROJECT_NAME})
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE mingw32
                                                  SDL2main
                                                  SDL2
                                                  opengl32
                                                  -mwindows
                                                  )
elseif(APPLE)
    add_executable(${PROJECT_NAME})
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main
                                              SDL2::SDL2-static
                                              "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(${PROJECT_NAME})
    find_package(SDL2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main GL stdc++fs)
elseif(MSVC)
    add_executable(${PROJECT_NAME})
    find_package(SDL2 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glm
                                                  SDL2::SDL2main
                                                  SDL2::SDL2
                                                  opengl32
                                                  )
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_sources(${PROJECT_NAME} PRIVATE main.cxx
                                       gles30_shader.cxx
                                       opengles30.cxx
                                       gles30_texture.cxx
                                       properties_reader.cxx
                                       fps_camera.cxx
                                       opengles30.hxx
                                       gles30_shader.hxx
                                       gles30_texture.hxx
                                       properties_reader.hxx
                                       fps_camera.hxx
                                       stb_image.h

                                       res/basic.fsh
                                       res/basic.vsh
                                       res/material.fsh
                                       res/vertex_pos.vsh

                                       res/runtime.properties.hxx
                                       )


if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4  /utf-8 ") # /WX
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unknown-pragmas")
endif()
