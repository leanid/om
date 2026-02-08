add_executable(02-vk-instance main.cxx)

target_compile_definitions(02-vk-instance PRIVATE VK_NULL_HANDLE=nullptr)

target_compile_features(02-vk-instance PRIVATE cxx_std_20)
target_include_directories(
    02-vk-instance
    PRIVATE
        $<$<PLATFORM_ID:Windows,Linux,Darvin>:${CMAKE_SOURCE_DIR}/support/cxx_lib/>
    )
target_link_libraries(02-vk-instance PRIVATE Vulkan::Headers Vulkan::Vulkan
                                             SDL3::SDL3-shared)
