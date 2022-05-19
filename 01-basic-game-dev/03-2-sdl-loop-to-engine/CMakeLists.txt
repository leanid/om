cmake_minimum_required(VERSION 3.16)
project(03-sdl-loop-to-engine)

add_executable(03-sdl-loop-to-engine
    main.cxx
    engine.cxx
    engine.hxx)
target_compile_features(03-sdl-loop-to-engine PUBLIC cxx_std_17)

find_package(SDL2 REQUIRED)

target_link_libraries(03-sdl-loop-to-engine PRIVATE SDL2::SDL2 SDL2::SDL2main)


install(TARGETS 03-sdl-loop-to-engine
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
