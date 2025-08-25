add_library(10-vk-ubo-args)
target_sources(
    10-vk-ubo-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(10-vk-ubo-args PRIVATE Boost::program_options)

add_library(10-vk-ubo-vulkan STATIC)
target_sources(
    10-vk-ubo-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    10-vk-ubo-vulkan
    PUBLIC Boost::stacktrace
           SDL3::SDL3-shared
           vulkan::cppm
           glm::cppm
           10-vk-ubo-args
           10-vk-ubo-log)
target_include_directories(10-vk-ubo-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)
