add_library(09-vk-res-loading-1-sdl STATIC)
target_sources(
    09-vk-res-loading-1-sdl
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL.g.cxx")
target_link_libraries(09-vk-res-loading-1-sdl PRIVATE SDL3::SDL3-shared)

add_library(09-vk-res-loading-1-sdl-vulkan STATIC)
target_sources(
    09-vk-res-loading-1-sdl-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL_vulkan.g.cxx")
target_link_libraries(09-vk-res-loading-1-sdl-vulkan PRIVATE SDL3::SDL3-shared)
