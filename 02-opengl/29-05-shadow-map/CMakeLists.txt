cmake_minimum_required(VERSION 3.20...3.22)

if(SDL2_SRC_DIR) # Android build directly from source
    add_subdirectory(
        # Specifies the directory of the CMakeLists.txt file.
        ${SDL2_SRC_DIR}
        # Specifies the directory for the build outputs.
        ${SDL2_BUILD_DIR}_${ANDROID_ABI}
    )
endif()

# super fast way to go on linux with assimp: sudo dnf install assimp-devel or
# build it yourself for android for example: sudo dnf install glm-devel

project(29-05-shadow-map CXX)

if(SDL2_SRC_DIR) # Android
    add_library(29-05-shadow-map shared)
    find_library(
        SDL2_LIB
        NAMES SDL2
    )
    target_include_directories(
        29-05-shadow-map
        PRIVATE ${SDL2_SRC_DIR}/include
    )
    target_link_libraries(
        29-05-shadow-map
        PRIVATE SDL2 SDL2main
    )
elseif(MINGW)
    add_executable(29-05-shadow-map)
    find_package(SDL2 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(
        29-05-shadow-map
        PRIVATE mingw32
                glm
                assimp
                SDL2::SDL2main
                SDL2::SDL2
                opengl32
                -mwindows
    )
elseif(APPLE)
    add_executable(29-05-shadow-map)
    find_package(
        SDL2
        CONFIG
        REQUIRED
    )
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    # HACK enable debug
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -g -O0"
    )
    target_link_libraries(
        29-05-shadow-map
        PRIVATE SDL2::SDL2main
                SDL2::SDL2-static
                glm::glm
                assimp::assimp
                "-framework OpenGL"
    )
elseif(UNIX)
    add_executable(29-05-shadow-map)
    find_package(SDL2 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(
        29-05-shadow-map
        PRIVATE SDL2
                ${ASSIMP_LIBRARIES}
                GL # stdc++fs -lz
    )
    target_include_directories(
        29-05-shadow-map
        PRIVATE ${SDL2_INCLUDE_DIRS}
    )
elseif(MSVC)
    add_executable(29-05-shadow-map)
    find_package(
        SDL2
        CONFIG
        REQUIRED
    )
    find_package(
        glm
        CONFIG
        REQUIRED
    )
    find_package(assimp REQUIRED)
    target_link_libraries(
        29-05-shadow-map
        PRIVATE glm
                ${ASSIMP_LIBRARIES}
                SDL2::SDL2main
                SDL2::SDL2
                opengl32
                IrrXMLd.lib
                zlibstaticd.lib
    )
    set(ASSIMP_POSTFIX
        "-vc140-mtd.lib"
    )
endif()

target_include_directories(
    29-05-shadow-map SYSTEM
    PRIVATE ${CMAKE_BINARY_DIR}/gsl_inc
)

target_compile_features(
    29-05-shadow-map
    PUBLIC cxx_std_20
)

target_sources(
    29-05-shadow-map
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
            stb_image.h
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
    29-05-shadow-map
    PRIVATE glad_29_5
)
