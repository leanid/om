file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(imgui STATIC ${sources} ${headers})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
