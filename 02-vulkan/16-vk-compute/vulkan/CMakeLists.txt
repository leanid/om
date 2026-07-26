add_library(16-vk-compute-args)
target_sources(
    16-vk-compute-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(16-vk-compute-args PRIVATE Boost::program_options)

add_library(16-vk-compute-vulkan STATIC)
target_sources(
    16-vk-compute-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    16-vk-compute-vulkan
    PUBLIC
        $<IF:$<CXX_COMPILER_ID:MSVC>,Boost::stacktrace_basic,Boost::stacktrace>
        SDL3::SDL3-shared
        vulkan::cppm
        glm::cppm
        stb::stb
        16-vk-compute-args
        16-vk-compute-log)
target_include_directories(16-vk-compute-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)