add_library(13-vk-model-args)
target_sources(
    13-vk-model-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(13-vk-model-args PRIVATE Boost::program_options)

add_library(13-vk-model-vulkan STATIC)
target_sources(
    13-vk-model-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    13-vk-model-vulkan
    PUBLIC
        $<IF:$<CXX_COMPILER_ID:MSVC>,Boost::stacktrace_basic,Boost::stacktrace>
        SDL3::SDL3-shared
        vulkan::cppm
        glm::cppm
        stb::stb
        13-vk-model-args
        13-vk-model-log)
target_include_directories(13-vk-model-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)
