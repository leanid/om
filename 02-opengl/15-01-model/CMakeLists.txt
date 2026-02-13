cmake_minimum_required(VERSION 3.16.0)

if(SDL3_SRC_DIR) # Android build directly from source
    add_subdirectory(
        # Specifies the directory of the CMakeLists.txt file.
        ${SDL3_SRC_DIR}
        # Specifies the directory for the build outputs.
        ${SDL3_BUILD_DIR}_${ANDROID_ABI})
endif()

project(15-01-model CXX)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)

if(SDL3_SRC_DIR) # Android
    add_library(15-01-model SHARED)
    target_include_directories(15-01-model PRIVATE ${SDL3_SRC_DIR}/include)
    target_link_libraries(15-01-model PRIVATE SDL3::SDL3-shared)
elseif(MINGW)
    add_executable(15-01-model)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(15-01-model PRIVATE SDL3::SDL3-shared OpenGL::GL)
elseif(APPLE)
    add_executable(15-01-model)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(15-01-model PRIVATE SDL3::SDL3-shared
                                              "-framework OpenGL")
elseif(UNIX)
    add_executable(15-01-model)
    find_package(SDL3 REQUIRED)
    target_link_libraries(15-01-model PRIVATE SDL3::SDL3-shared GL)
elseif(MSVC)
    add_executable(15-01-model)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(
        15-01-model
        PRIVATE SDL3::SDL3-shared
                opengl32
                IrrXMLd.lib
                zlibstaticd.lib)
endif()

target_compile_features(15-01-model PUBLIC cxx_std_20)

file(GLOB MODEL_PNG_FILES RELATIVE ${CMAKE_CURRENT_LIST_DIR} res/model/*.png)

target_sources(
    15-01-model
    PRIVATE main.cxx
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
            res/nanosuit.fsh
            res/nanosuit.vsh
            res/light_cube.vsh
            res/light_cube.fsh
            res/runtime.properties.hxx
            res/container2.png
            res/container2_specular.png
            ${MODEL_PNG_FILES})

target_link_libraries(15-01-model PRIVATE stb::stb)
target_link_libraries(15-01-model PRIVATE assimp::assimp)
target_link_libraries(15-01-model PRIVATE glm::glm)
