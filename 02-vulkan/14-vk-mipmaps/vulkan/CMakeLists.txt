add_library(14-vk-mipmaps-args)
target_sources(
    14-vk-mipmaps-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(14-vk-mipmaps-args PRIVATE Boost::program_options)

add_library(14-vk-mipmaps-vulkan STATIC)
target_sources(
    14-vk-mipmaps-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    14-vk-mipmaps-vulkan
    PUBLIC
        $<IF:$<CXX_COMPILER_ID:MSVC>,Boost::stacktrace_basic,Boost::stacktrace>
        SDL3::SDL3-shared
        vulkan::cppm
        glm::cppm
        stb::stb
        14-vk-mipmaps-args
        14-vk-mipmaps-log)
target_include_directories(14-vk-mipmaps-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)