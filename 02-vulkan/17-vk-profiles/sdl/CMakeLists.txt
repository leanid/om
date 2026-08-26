add_library(17-vk-profiles-sdl STATIC)
target_sources(
    17-vk-profiles-sdl
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL.g.cxx")
target_link_libraries(17-vk-profiles-sdl PRIVATE SDL3::SDL3-shared)

add_library(17-vk-profiles-sdl-vulkan STATIC)
target_sources(
    17-vk-profiles-sdl-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL_vulkan.g.cxx")
target_link_libraries(17-vk-profiles-sdl-vulkan PRIVATE SDL3::SDL3-shared)