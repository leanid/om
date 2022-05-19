file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(imgui_editor STATIC ${sources} ${headers})
target_include_directories(imgui_editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET imgui_editor PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(imgui_editor PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS=1)
