cmake_minimum_required(VERSION 3.16.0)

if(SDL3_SRC_DIR)  # Android build directly from source
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL3_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL3_BUILD_DIR}_${ANDROID_ABI} )
endif()

# super fast way to go on linux with assimp:
# sudo dnf install assimp-devel
# or
# build it yourself for android for example:
# sudo dnf install glm-devel

include(ExternalProject)

ExternalProject_Add(microsoft_gsl
  GIT_REPOSITORY    https://github.com/microsoft/GSL.git
  GIT_TAG           main
  GIT_PROGRESS      TRUE
  CMAKE_CACHE_ARGS  -DGSL_CXX_STANDARD:STRING=c++17
                    -DGSL_TEST:STRING=OFF
                    -DCMAKE_INSTALL_INCLUDEDIR:PATH=${CMAKE_BINARY_DIR}/gsl_inc
		    -DCMAKE_INSTALL_DATADIR:PATH=${CMAKE_BINARY_DIR}/gsl_inc
  )

project(23-03-uniform-buffers CXX)

if (SDL3_SRC_DIR) # Android
    add_library(23-03-uniform-buffers SHARED)
    target_include_directories(23-03-uniform-buffers PRIVATE ${SDL3_SRC_DIR}/include )
    target_link_libraries(23-03-uniform-buffers PRIVATE SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(23-03-uniform-buffers)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(23-03-uniform-buffers PRIVATE SDL3::SDL3-shared mingw32
                                                      glm
                                                      assimp
                                                      opengl32
                                                      -mwindows
                                                      )
elseif(APPLE)
    add_executable(23-03-uniform-buffers)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(23-03-uniform-buffers PRIVATE SDL3::SDL3-shared "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(23-03-uniform-buffers)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(23-03-uniform-buffers PRIVATE SDL3::SDL3-shared ${ASSIMP_LIBRARIES}
                                                  GL
                                                  stdc++fs
                                                  -lz
                                                  )
    target_include_directories(23-03-uniform-buffers PRIVATE ${SDL3_INCLUDE_DIRS})
elseif(MSVC)
    add_executable(23-03-uniform-buffers)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(assimp REQUIRED)
    target_link_libraries(23-03-uniform-buffers PRIVATE SDL3::SDL3-shared glm
                                                  ${ASSIMP_LIBRARIES}
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

add_dependencies(23-03-uniform-buffers microsoft_gsl)

target_include_directories(23-03-uniform-buffers SYSTEM PRIVATE ${CMAKE_BINARY_DIR}/gsl_inc)

target_compile_features(23-03-uniform-buffers PUBLIC cxx_std_20)

target_sources(23-03-uniform-buffers PRIVATE main.cxx
                                   gles30_shader.hxx
                                   gles30_shader.cxx
                                   gles30_texture.hxx
                                   gles30_texture.cxx
                                   gles30_mesh.hxx
                                   gles30_mesh.cxx
                                   gles30_model.hxx
                                   gles30_model.cxx
                                   gles30_framebuffer.hxx
                                   gles30_framebuffer.cxx

                                   properties_reader.hxx
                                   properties_reader.cxx
                                   fps_camera.hxx
                                   fps_camera.cxx
                                   opengles30.hxx
                                   opengles30.cxx

                                   res/cube.vsh
                                   res/cube.fsh
                                   res/outline.fsh
                                   res/discard.fsh
                                   res/quad_vertex.vsh
                                   res/quad_frag.fsh
                                   res/skybox.vsh
                                   res/skybox.fsh
                                   res/refraction.vsh
                                   res/refraction.fsh

                                   res/runtime.properties.hxx

                                   res/marble.jpg
                                   res/metal.png
                                   res/grass.png
                                   res/blending_transparent_window.png

                                   res/skybox/right.jpg
                                   res/skybox/left.jpg
                                   res/skybox/top.jpg
                                   res/skybox/bottom.jpg
                                   res/skybox/front.jpg
                                   res/skybox/back.jpg

                                   )

add_subdirectory(glad)
target_link_libraries(23-03-uniform-buffers PRIVATE glad_23_3 stb::stb)

