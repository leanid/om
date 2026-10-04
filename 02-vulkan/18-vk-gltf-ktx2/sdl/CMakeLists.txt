add_library(18-vk-gltf-ktx2-sdl STATIC)
target_sources(
    18-vk-gltf-ktx2-sdl
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL.g.cxx")
target_link_libraries(18-vk-gltf-ktx2-sdl PRIVATE SDL3::SDL3-shared)

add_library(18-vk-gltf-ktx2-sdl-vulkan STATIC)
target_sources(
    18-vk-gltf-ktx2-sdl-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL_vulkan.g.cxx")
target_link_libraries(18-vk-gltf-ktx2-sdl-vulkan PRIVATE SDL3::SDL3-shared)