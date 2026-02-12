cmake_minimum_required(VERSION 3.14.0)

if(SDL3_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL3_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL3_BUILD_DIR}_${ANDROID_ABI} )
endif()

project(14-01-mesh CXX)
find_package(assimp REQUIRED)

if (SDL3_SRC_DIR) # Android
    add_library(${PROJECT_NAME} SHARED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_SRC_DIR}/include )
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared mingw32
                                                  opengl32
                                                  -mwindows
                                                  )
elseif(APPLE)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared GL stdc++fs)
elseif(MSVC)
    add_executable(${PROJECT_NAME})
    find_package(SDL3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared glm
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_sources(${PROJECT_NAME} PRIVATE main.cxx
                                       gles30_shader.hxx
                                       gles30_shader.cxx
                                       opengles30.hxx
                                       opengles30.cxx
                                       gles30_texture.hxx
                                       gles30_texture.cxx
                                       properties_reader.hxx
                                       properties_reader.cxx
                                       fps_camera.hxx
                                       fps_camera.cxx
                                       gles30_mesh.hxx
                                       gles30_mesh.cxx

                                       res/basic.fsh
                                       res/basic.vsh
                                       res/material.fsh
                                       res/vertex_pos.vsh

                                       res/runtime.properties.hxx
                                       )

target_link_libraries(14-01-mesh PRIVATE stb::stb)

target_link_libraries(14-01-mesh PRIVATE assimp::assimp)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4  /utf-8 ") # /WX
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unknown-pragmas")
endif()
