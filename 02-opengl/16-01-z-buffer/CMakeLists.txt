cmake_minimum_required(VERSION 3.16.0)

if(SDL3_SRC_DIR) # Android build directly from source
    add_subdirectory(
        # Specifies the directory of the CMakeLists.txt file.
        ${SDL3_SRC_DIR}
        # Specifies the directory for the build outputs.
        ${SDL3_BUILD_DIR}_${ANDROID_ABI})
endif()

# super fast way to go on linux with assimp:
# sudo dnf install assimp-devel
# or
# build it yourself for android for example:
# sudo dnf install glm-devel

project(16-01-z-buffer CXX)

if(SDL3_SRC_DIR) # Android
    add_library(16-01-z-buffer SHARED)
    target_include_directories(16-01-z-buffer PRIVATE ${SDL3_SRC_DIR}/include)
    target_link_libraries(16-01-z-buffer PRIVATE SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(16-01-z-buffer)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(
        16-01-z-buffer
        PRIVATE SDL3::SDL3-shared mingw32
                opengl32
                -mwindows)
elseif(APPLE)
    add_executable(16-01-z-buffer)
    find_package(SDL3 CONFIG REQUIRED)
    target_link_libraries(16-01-z-buffer PRIVATE SDL3::SDL3-shared "-framework OpenGL")
elseif(UNIX)
    add_executable(16-01-z-buffer)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED) # sudo dnf install assimp-devel
    find_package(glm REQUIRED)
    target_link_libraries(
        16-01-z-buffer
        PRIVATE SDL3::SDL3-shared ${ASSIMP_LIBRARIES}
                GL
                stdc++fs
                -lz)
elseif(MSVC)
    add_executable(16-01-z-buffer)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(assimp CONFIG REQUIRED)
    target_link_libraries(
        16-01-z-buffer
        PRIVATE SDL3::SDL3-shared glm
                ${ASSIMP_LIBRARIES}
                opengl32
                IrrXMLd.lib
                zlibstaticd.lib)
    set(ASSIMP_POSTFIX "-vc140-mtd.lib")
endif()

target_compile_features(16-01-z-buffer PUBLIC cxx_std_20)

file(GLOB MODEL_PNG_FILES RELATIVE ${CMAKE_CURRENT_LIST_DIR} res/model/*.png)

target_sources(
    16-01-z-buffer
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
            res/cube.vsh
            res/cube.fsh
            res/runtime.properties.hxx
            res/marble.jpg
            res/metal.png
            ${MODEL_PNG_FILES})

target_link_libraries(16-01-z-buffer PRIVATE stb::stb)
