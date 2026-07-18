add_library(15-vk-multisampling-args)
target_sources(
    15-vk-multisampling-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(15-vk-multisampling-args PRIVATE Boost::program_options)

add_library(15-vk-multisampling-vulkan STATIC)
target_sources(
    15-vk-multisampling-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    15-vk-multisampling-vulkan
    PUBLIC
        $<IF:$<CXX_COMPILER_ID:MSVC>,Boost::stacktrace_basic,Boost::stacktrace>
        SDL3::SDL3-shared
        vulkan::cppm
        glm::cppm
        stb::stb
        15-vk-multisampling-args
        15-vk-multisampling-log)
target_include_directories(15-vk-multisampling-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/)