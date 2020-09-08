cmake_minimum_required(VERSION 3.16.0)

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

set(ASSIMP_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build/assimp_root)

ExternalProject_Add(assimp_proj_15_1
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

ExternalProject_Add(glm_proj_15_1
  PREFIX            ${ASSIMP_BUILD_DIR}
  GIT_REPOSITORY    https://github.com/g-truc/glm.git
  GIT_TAG           0.9.9.5
  GIT_PROGRESS      TRUE
  CMAKE_CACHE_ARGS  -DBUILD_SHARED_LIBS:BOOL=OFF
                    -DBUILD_STATIC_LIBS:BOOL=OFF
                    -DGLM_TEST_ENABLE:BOOL=OFF
                    -DCMAKE_INSTALL_PREFIX:PATH=${ASSIMP_BUILD_DIR}_install/
)

project(15-01-model CXX)

if (SDL2_SRC_DIR) # Android
    add_library(15-01-model SHARED)
    find_library(SDL2_LIB NAMES SDL2)
    target_include_directories(15-01-model PRIVATE ${SDL2_SRC_DIR}/include )
    target_link_libraries(15-01-model PRIVATE SDL2 SDL2main )
elseif(MINGW)
    add_executable(15-01-model)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(15-01-model PRIVATE mingw32
                                                  SDL2main
                                                  SDL2
                                                  opengl32
                                                  -mwindows
                                                  )
elseif(APPLE)
    add_executable(15-01-model)
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(15-01-model PRIVATE SDL2::SDL2main
                                              SDL2::SDL2-static
                                              "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(15-01-model)
    find_package(SDL2 REQUIRED)
    target_link_libraries(15-01-model PRIVATE SDL2
                                                  GL
                                                  stdc++fs
                                                  -lz
                                                  )
elseif(MSVC)
    add_executable(15-01-model)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    target_link_libraries(15-01-model PRIVATE glm
                                                  SDL2::SDL2main
                                                  SDL2::SDL2
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

target_compile_features(15-01-model PUBLIC cxx_std_17)

file(GLOB MODEL_PNG_FILES
     RELATIVE ${CMAKE_CURRENT_LIST_DIR}
     res/model/*.png)

target_sources(15-01-model PRIVATE main.cxx
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
                                   gles30_model.hxx
                                   gles30_model.cxx
                                   stb_image.h

                                   res/nanosuit.fsh
                                   res/nanosuit.vsh
                                   res/light_cube.vsh
                                   res/light_cube.fsh

                                   res/runtime.properties.hxx

                                   res/container2.png
                                   res/container2_specular.png

                                   ${MODEL_PNG_FILES}
                                   )

add_dependencies(15-01-model assimp_proj_15_1)
add_dependencies(15-01-model glm_proj_15_1)

target_link_directories(15-01-model PRIVATE ${ASSIMP_BUILD_DIR}_install/lib)
target_link_libraries(15-01-model PRIVATE assimp${ASSIMP_POSTFIX} IrrXML)
target_include_directories(15-01-model SYSTEM PRIVATE ${ASSIMP_BUILD_DIR}_install/include)
target_link_libraries(15-01-model INTERFACE glm)
