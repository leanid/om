add_library(20-vk-multithreading-args)
target_sources(
    20-vk-multithreading-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(20-vk-multithreading-args PRIVATE Boost::program_options)

add_library(20-vk-multithreading-vulkan STATIC)
target_sources(
    20-vk-multithreading-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    20-vk-multithreading-vulkan
    PUBLIC
        $<IF:$<CXX_COMPILER_ID:MSVC>,Boost::stacktrace_basic,Boost::stacktrace>
        SDL3::SDL3-shared
        vulkan::cppm
        glm::cppm
        stb::stb
        KTX::ktx
        20-vk-multithreading-args
        20-vk-multithreading-log)
target_include_directories(20-vk-multithreading-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                                   "${Vulkan_INCLUDE_DIR}")
