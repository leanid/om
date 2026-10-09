add_library(18-vk-gltf-ktx2-args)
target_sources(
    18-vk-gltf-ktx2-args
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/args_parser.cxx")

target_link_libraries(18-vk-gltf-ktx2-args PRIVATE Boost::program_options)

add_library(18-vk-gltf-ktx2-vulkan STATIC)
target_sources(
    18-vk-gltf-ktx2-vulkan
    PUBLIC FILE_SET
           cxx_modules
           TYPE
           CXX_MODULES
           BASE_DIRS
           "${CMAKE_CURRENT_SOURCE_DIR}"
           FILES
           "${CMAKE_CURRENT_SOURCE_DIR}/render.cxx")

target_link_libraries(
    18-vk-gltf-ktx2-vulkan
    PUBLIC
        $<IF:$<CXX_COMPILER_ID:MSVC>,Boost::stacktrace_basic,Boost::stacktrace>
        SDL3::SDL3-shared
        vulkan::cppm
        glm::cppm
        stb::stb
        KTX::ktx
        18-vk-gltf-ktx2-args
        18-vk-gltf-ktx2-log)
target_include_directories(18-vk-gltf-ktx2-vulkan
                           PRIVATE ${CMAKE_SOURCE_DIR}/support/cxx_lib/
                                   "${Vulkan_INCLUDE_DIR}")
