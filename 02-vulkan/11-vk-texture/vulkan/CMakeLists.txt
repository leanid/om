add_library(11-vk-texture-args)
target_sources(
    11-vk-texture-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(11-vk-texture-args PRIVATE Boost::program_options)

add_library(11-vk-texture-vulkan STATIC)
target_sources(
    11-vk-texture-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    11-vk-texture-vulkan
    PUBLIC Boost::stacktrace
           SDL3::SDL3-shared
           vulkan::cppm
           glm::cppm
           11-vk-texture-args
           11-vk-texture-log)
target_include_directories(11-vk-texture-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)
