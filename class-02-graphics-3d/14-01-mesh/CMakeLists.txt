cmake_minimum_required(VERSION 3.14.0)

if(SDL2_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL2_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL2_BUILD_DIR}_${ANDROID_ABI} )
endif()

include(ExternalProject)

# super fast way to go on linux with assimp:
# sudo dnf install assimp-devel
# or
# build it yourself for android for example:

set(ASSIMP_BUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/build/assimp_root)

ExternalProject_Add(assimp_proj
  PREFIX            ${ASSIMP_BUILD_DIR}
  GIT_REPOSITORY    https://github.com/assimp/assimp.git
  GIT_TAG           v.5.0.0.rc1
  GIT_PROGRESS      TRUE
  CMAKE_CACHE_ARGS  -DBUILD_SHARED_LIBS:STRING=OFF
                    -DASSIMP_NO_EXPORT:STRING=ON
                    -DASSIMP_BUILD_ASSIMP_TOOLS:STRING=OFF
                    -DASSIMP_BUILD_TESTS:STRING=OFF
                    -DASSIMP_INSTALL_PDB:STRING=OFF
                    -DINJECT_DEBUG_POSTFIX:STRING=OFF
                    -DCMAKE_INSTALL_PREFIX:PATH=${ASSIMP_BUILD_DIR}_install/
)

project(14-01-mesh CXX)

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
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 GL stdc++fs)
elseif(MSVC)
    add_executable(${PROJECT_NAME})
    find_package(SDL2 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glm
                                                  SDL2::SDL2main
                                                  SDL2::SDL2
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
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
                                       stb_image.h

                                       res/basic.fsh
                                       res/basic.vsh
                                       res/material.fsh
                                       res/vertex_pos.vsh

                                       res/runtime.properties.hxx
                                       )

add_dependencies(${PROJECT_NAME} assimp_proj)

target_link_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_BUILD_DIR}_install/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp${ASSIMP_POSTFIX})
target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_BUILD_DIR}_install/include)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4  /utf-8 ") # /WX
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-unknown-pragmas")
endif()
