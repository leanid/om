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


install(TARGETS 04-0-render-basic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
