add_library(15-vk-multisampling-sdl STATIC)
target_sources(
    15-vk-multisampling-sdl
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL.g.cxx")
target_link_libraries(15-vk-multisampling-sdl PRIVATE SDL3::SDL3-shared)

add_library(15-vk-multisampling-sdl-vulkan STATIC)
target_sources(
    15-vk-multisampling-sdl-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL_vulkan.g.cxx")
target_link_libraries(15-vk-multisampling-sdl-vulkan PRIVATE SDL3::SDL3-shared)