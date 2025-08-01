add_library(09-vk-res-loading-1-args OBJECT args_parser.cxx args_parser.hxx)
target_link_libraries(09-vk-res-loading-1-args PUBLIC Boost::program_options)

add_library(
    09-vk-res-loading-1-vulkan STATIC
    mesh.cxx
    mesh.hxx
    platform_sdl3.cxx
    platform_sdl3.hxx
    render.cxx
    render.hxx)

target_link_libraries(
    09-vk-res-loading-1-vulkan
    PUBLIC SDL3::SDL3-shared
           Vulkan::Headers
           Vulkan::Vulkan
           glm::glm
           om::io::read_file
           09-vk-res-loading-1-args
           09-vk-res-loading-1-log)
target_include_directories(09-vk-res-loading-1-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)
