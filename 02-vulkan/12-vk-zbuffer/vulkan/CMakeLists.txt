add_library(12-vk-texture-args)
target_sources(
    12-vk-texture-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(12-vk-texture-args PRIVATE Boost::program_options)

add_library(12-vk-texture-vulkan STATIC)
target_sources(
    12-vk-texture-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    12-vk-texture-vulkan
    PUBLIC
        $<IF:$<CXX_COMPILER_ID:MSVC>,Boost::stacktrace_basic,Boost::stacktrace>
        SDL3::SDL3-shared
        vulkan::cppm
        glm::cppm
        stb::stb
        12-vk-texture-args
        12-vk-texture-log)
target_include_directories(12-vk-texture-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)
