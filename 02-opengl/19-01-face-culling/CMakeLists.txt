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

project(19-01-face-culling CXX)

if (SDL3_SRC_DIR) # Android
    add_library(19-01-face-culling SHARED)
    target_include_directories(19-01-face-culling PRIVATE ${SDL3_SRC_DIR}/include )
    target_link_libraries(19-01-face-culling PRIVATE SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(19-01-face-culling)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(19-01-face-culling PRIVATE SDL3::SDL3-shared mingw32
                                                  opengl32
                                                  -mwindows
                                                  )
elseif(APPLE)
    add_executable(19-01-face-culling)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(19-01-face-culling PRIVATE SDL3::SDL3-shared "-framework OpenGL"
                                              )
elseif(UNIX)
    add_executable(19-01-face-culling)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(19-01-face-culling PRIVATE SDL3::SDL3-shared ${ASSIMP_LIBRARIES}
                                                  GL
                                                  -lz
                                                  )
elseif(MSVC)
    add_executable(19-01-face-culling)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(assimp REQUIRED)
    target_link_libraries(19-01-face-culling PRIVATE SDL3::SDL3-shared glm
                                                  ${ASSIMP_LIBRARIES}
                                                  opengl32
                                                  IrrXMLd.lib
                                                  zlibstaticd.lib
                                                  )
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

target_compile_features(19-01-face-culling PUBLIC cxx_std_20)

target_sources(19-01-face-culling PRIVATE main.cxx
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

                                   res/cube.vsh
                                   res/cube.fsh
                                   res/outline.fsh
                                   res/discard.fsh

                                   res/runtime.properties.hxx

                                   res/marble.jpg
                                   res/metal.png
                                   res/grass.png
                                   res/blending_transparent_window.png
                                   )

target_link_libraries(19-01-face-culling PRIVATE stb::stb)

