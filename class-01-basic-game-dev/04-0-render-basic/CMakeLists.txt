cmake_minimum_required(VERSION 3.16)
project(04-0-render-basic CXX)

add_executable(04-0-render-basic 00_canvas_basic.cxx
                                 00_canvas_basic.hxx
                                 00_canvas_basic_main.cxx
                                 )
target_compile_features(04-0-render-basic PUBLIC cxx_std_17)

add_executable(04-0-render-basic-line 01_line_render.cxx
                                      01_line_render.hxx
                                      01_line_render_main.cxx
                                      00_canvas_basic.cxx
                                      )
target_compile_features(04-0-render-basic-line PUBLIC cxx_std_17)

add_executable(04-0-render-basic-triangle 02_triangle_render.cxx
                                          02_triangle_render.hxx
                                          02_triangle_render_main.cxx
                                          00_canvas_basic.cxx
                                          01_line_render.cxx
                                          )
target_compile_features(04-0-render-basic-triangle PUBLIC cxx_std_17)

add_executable(04-0-render-basic-triangle-indexed 03_triangle_indexed_render.cxx
                                                  03_triangle_indexed_render.hxx
                                                  03_triangle_indexed_render_main.cxx
                                                  00_canvas_basic.cxx
                                                  01_line_render.cxx
                                                  02_triangle_render.cxx
                                                  )
target_compile_features(04-0-render-basic-triangle-indexed PUBLIC cxx_std_17)

add_executable(04-0-render-basic-triangle-interpolated 04_triangle_interpolated_render.cxx
                                                       04_triangle_interpolated_render.hxx
                                                       04_triangle_interpolated_render_main.cxx
                                                       00_canvas_basic.cxx
                                                       01_line_render.cxx
                                                       02_triangle_render.cxx
                                                       03_triangle_indexed_render.cxx
                                                       )
target_compile_features(04-0-render-basic-triangle-interpolated PUBLIC cxx_std_17)

find_package(SDL2 REQUIRED)
message(STATUS "SDL2_LIBRARIES[${SDL2_LIBRARIES}] "
    "SDL2_INCLUDE_DIRS[${SDL2_INCLUDE_DIRS}]")


add_executable(04-0-render-basic-windowed 05-0-render-basic-windowed.cxx
                                          04_triangle_interpolated_render.cxx
                                          04_triangle_interpolated_render.hxx
                                          00_canvas_basic.cxx
                                          01_line_render.cxx
                                          02_triangle_render.cxx
                                          03_triangle_indexed_render.cxx
                                          )
target_link_libraries(04-0-render-basic-windowed PRIVATE ${SDL2_LIBRARIES})
target_include_directories(04-0-render-basic-windowed PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(04-0-render-basic-windowed PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_compile_features(04-0-render-basic-windowed PUBLIC cxx_std_17)

add_executable(04-0-render-full-windowed  04-0-render-full-windowed.cxx
                                          04_triangle_interpolated_render.cxx
                                          04_triangle_interpolated_render.hxx
                                          00_canvas_basic.cxx
                                          01_line_render.cxx
                                          02_triangle_render.cxx
                                          03_triangle_indexed_render.cxx
                                          )
target_link_libraries(04-0-render-full-windowed PRIVATE ${SDL2_LIBRARIES})
target_include_directories(04-0-render-full-windowed PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(04-0-render-full-windowed PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_compile_features(04-0-render-full-windowed PUBLIC cxx_std_17)

add_executable(04-0-render-image-windowed 04-0-image-windowed.cxx
                                          04_triangle_interpolated_render.cxx
                                          04_triangle_interpolated_render.hxx
                                          00_canvas_basic.cxx
                                          01_line_render.cxx
                                          02_triangle_render.cxx
                                          03_triangle_indexed_render.cxx
                                          )
target_link_libraries(04-0-render-image-windowed PRIVATE ${SDL2_LIBRARIES})
target_include_directories(04-0-render-image-windowed PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(04-0-render-image-windowed PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_compile_features(04-0-render-image-windowed PUBLIC cxx_std_17)

add_executable(04-06-ray-tracing-basic 06_ray_tracing_basic.cxx
                                       00_canvas_basic.hxx
                                       00_canvas_basic.cxx)
target_compile_features(04-06-ray-tracing-basic PRIVATE cxx_std_17)

add_executable(04-07-ray-tracing-basic 07_ray_tracing_basic.cxx
                                       00_canvas_basic.hxx
                                       00_canvas_basic.cxx)
target_compile_features(04-07-ray-tracing-basic PRIVATE cxx_std_17)

add_executable(04-08-ray-tracing-basic 08_ray_tracing_basic.cxx
                                       00_canvas_basic.hxx
                                       00_canvas_basic.cxx)
target_compile_features(04-08-ray-tracing-basic PRIVATE cxx_std_17)


install(TARGETS 04-0-render-basic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
