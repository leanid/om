cmake_minimum_required(VERSION 3.6)

if(SDL3_SRC_DIR)  # Android build directly from source
    set(SDL_DISABLE_IMMINTRIN_H 1)
    add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
                      ${SDL3_SRC_DIR}
                      # Specifies the directory for the build outputs.
                      ${SDL3_BUILD_DIR} )
endif()


project(00-4-prepare-android-gles30 CXX)

add_library(${PROJECT_NAME} SHARED main.cxx)
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if (SDL3_SRC_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_SRC_DIR}/include )
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared )
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17 /utf8")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -pedantic -Werror")
endif()
