add_library(20-vk-multithreading-sdl STATIC)
target_sources(
    20-vk-multithreading-sdl
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL.g.cxx")
target_link_libraries(20-vk-multithreading-sdl PRIVATE SDL3::SDL3-shared)

add_library(20-vk-multithreading-sdl-vulkan STATIC)
target_sources(
    20-vk-multithreading-sdl-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/SDL_vulkan.g.cxx")
target_link_libraries(20-vk-multithreading-sdl-vulkan PRIVATE SDL3::SDL3-shared)