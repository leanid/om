cmake_minimum_required(VERSION 3.31)

project(38-import-cxx)

find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(38-import-cxx main.cxx)

target_sources(
    38-import-cxx
    PRIVATE FILE_SET
            modules
            TYPE
            CXX_MODULES
            FILES
            glm.cppm
            vulkan.cppm)

target_link_libraries(38-import-cxx PRIVATE glm::glm Vulkan::Headers)
