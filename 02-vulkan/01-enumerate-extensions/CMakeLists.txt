add_executable(01-vk-enumerate-extensions main.cxx)

target_include_directories(01-vk-enumerate-extensions PRIVATE Vulkan::Headers)
target_link_libraries(01-vk-enumerate-extensions PRIVATE Vulkan::Vulkan)
