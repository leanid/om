cmake_minimum_required(VERSION 3.16)
project(07-1-add-sound CXX)

add_executable(sound_test-07-1 sound_test.cxx)
target_compile_features(sound_test-07-1 PUBLIC cxx_std_20)

find_package(sdl2 REQUIRED)
target_include_directories(sound_test-07-1 PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(sound_test-07-1 PRIVATE ${SDL2_LIBRARIES})

if (MINGW)
    target_link_libraries(sound_test-07-1 PRIVATE
               -lmingw32
               -lSDL2main
               -lSDL2
               -mwindows
               -lopengl32
               )
elseif(MSVC)
    target_link_libraries(sound_test-07-1 PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()

install(TARGETS sound_test-07-1
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)

