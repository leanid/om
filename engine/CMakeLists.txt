cmake_minimum_required(VERSION 3.19...3.23)
project(engine LANGUAGES CXX)

find_package(SDL2 REQUIRED)

add_library(scanner SHARED src/fs_scanner.cxx src/fs_scanner.hxx)
target_link_libraries(scanner stdc++fs)
target_compile_features(scanner PRIVATE cxx_std_17)

add_executable(scanner_test src/fs_scanner_unit_test.cxx)
target_link_libraries(scanner_test scanner stdc++fs)
target_compile_features(scanner_test PRIVATE cxx_std_17)
find_path(DIRENT_INCLUDE_DIRS "dirent.h")

if(DIRENT_INCLUDE_DIRS)
    target_include_directories(scanner_test PRIVATE ${DIRENT_INCLUDE_DIRS})
endif()

add_executable(
    om
    include/om/engine.hxx
    include/om/game.hxx
    src/om/engine_impl.cxx
    src/om/main.cxx)

set_target_properties(om PROPERTIES ENABLE_EXPORTS TRUE)
target_include_directories(om PRIVATE include)
target_link_libraries(om PRIVATE SDL2::SDL2 SDL2::SDL2main stdc++fs)
target_compile_features(om PRIVATE cxx_std_17)

if(WIN32)
    target_compile_definitions(om PRIVATE "-DOM_EXP=__declspec(dllexport)")
endif()

install(TARGETS om RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
