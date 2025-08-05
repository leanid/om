add_library(09-vk-res-loading-1-args)
target_sources(
    09-vk-res-loading-1-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxxm")

target_link_libraries(09-vk-res-loading-1-args PRIVATE Boost::program_options)

add_library(
    09-vk-res-loading-1-vulkan STATIC
    mesh.cxx
    mesh.hxx
    render.cxx
    render.hxx)

target_link_libraries(
    09-vk-res-loading-1-vulkan
    PUBLIC SDL3::SDL3-shared
           Vulkan::Headers
           Vulkan::Vulkan
           glm::glm
           09-vk-res-loading-1-args
           09-vk-res-loading-1-log)
target_include_directories(09-vk-res-loading-1-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)
