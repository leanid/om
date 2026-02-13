cmake_minimum_required(VERSION 3.20...3.22)

if(SDL3_SRC_DIR) # Android build directly from source
    add_subdirectory(
        # Specifies the directory of the CMakeLists.txt file.
        ${SDL3_SRC_DIR}
        # Specifies the directory for the build outputs.
        ${SDL3_BUILD_DIR}_${ANDROID_ABI}
    )
endif()

# super fast way to go on linux with assimp: sudo dnf install assimp-devel or
# build it yourself for android for example: sudo dnf install glm-devel

project(30-1-point-shadow CXX)

if(SDL3_SRC_DIR) # Android
    add_library(30-1-point-shadow shared)
    target_include_directories(
        30-1-point-shadow
        PRIVATE ${SDL3_SRC_DIR}/include
    )
    target_link_libraries(
        30-1-point-shadow
        PRIVATE SDL3::SDL3-shared )
elseif(MINGW)
    add_executable(30-1-point-shadow)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(
        30-1-point-shadow
        PRIVATE SDL3::SDL3-shared mingw32
                glm
                assimp
                opengl32
                -mwindows
    )
elseif(APPLE)
    add_executable(30-1-point-shadow)
    find_package(SDL3 CONFIG
        REQUIRED
    )
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    # HACK enable debug
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -g -O0"
    )
    target_link_libraries(
        30-1-point-shadow
        PRIVATE SDL3::SDL3-shared glm::glm
                assimp::assimp
                "-framework OpenGL"
    )
elseif(UNIX)
    add_executable(30-1-point-shadow)
    find_package(SDL3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(
        30-1-point-shadow
        PRIVATE SDL3::SDL3-shared assimp::assimp
                GL
    )
    target_include_directories(
        30-1-point-shadow
        PRIVATE ${SDL3_INCLUDE_DIRS}
    )
elseif(MSVC)
    add_executable(30-1-point-shadow)
    find_package(SDL3 CONFIG
        REQUIRED
    )
    find_package(
        glm
        CONFIG
        REQUIRED
    )
    find_package(assimp REQUIRED)
    target_link_libraries(
        30-1-point-shadow
        PRIVATE SDL3::SDL3-shared glm
                assimp::assimp
                opengl32
                IrrXMLd.lib
                zlibstaticd.lib
    )
    set(ASSIMP_POSTFIX
        "-vc140-mtd.lib"
    )
endif()

target_include_directories(
    30-1-point-shadow SYSTEM
    PRIVATE ${CMAKE_BINARY_DIR}/gsl_inc
)

target_compile_features(
    30-1-point-shadow
    PUBLIC cxx_std_20
)

target_sources(
    30-1-point-shadow
    PRIVATE main.cxx
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
            res/instanced.vert
            res/instanced.frag
            res/textured.vert
            res/textured.frag
            res/quad.vert
            res/quad.frag
            res/mars.png
            res/planet.mtl
            res/rock.mtl
            res/depth.vert
            res/depth.frag
            res/shadow.vert
            res/shadow.frag
            res/runtime.properties.hxx
)

add_subdirectory(glad)
target_link_libraries(
    30-1-point-shadow
    PRIVATE glad_30_1
    stb::stb
)
