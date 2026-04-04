# Dear ImGui 1.92.x — single copy for basic-game-dev examples (core + SDL3 platform only).
# Rendering is done by each sample’s engine, not imgui_impl_opengl3.

find_package(SDL3 REQUIRED)

add_library(
    om_imgui STATIC
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui_demo.cpp
    backends/imgui_impl_sdl3.cpp)

target_include_directories(
    om_imgui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/backends>)

target_link_libraries(om_imgui PUBLIC SDL3::SDL3)

target_compile_features(om_imgui PUBLIC cxx_std_11)

set_property(TARGET om_imgui PROPERTY POSITION_INDEPENDENT_CODE ON)
