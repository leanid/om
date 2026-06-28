add_library(13-vk-model-sdl STATIC)
target_sources(
    13-vk-model-sdl
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL.g.cxx")
target_link_libraries(13-vk-model-sdl PRIVATE SDL3::SDL3-shared)

add_library(13-vk-model-sdl-vulkan STATIC)
target_sources(
    13-vk-model-sdl-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL_vulkan.g.cxx")
target_link_libraries(13-vk-model-sdl-vulkan PRIVATE SDL3::SDL3-shared)
