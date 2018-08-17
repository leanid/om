cmake_minimum_required(VERSION 3.9)
project(engine LANGUAGES CXX)

find_library(SDL2_LIB SDL2)

add_library(scanner SHARED src/fs_scanner.hxx
							src/fs_scanner.cxx)
target_link_libraries(scanner stdc++fs)
set_property(TARGET scanner PROPERTY CXX_STANDARD 17)							

add_executable(scanner_test src/fs_scanner_unit_test.cxx)
target_link_libraries(scanner_test scanner stdc++fs)
set_property(TARGET scanner_test PROPERTY CXX_STANDARD 17)

add_executable(om     src/om/main.cxx 
                      src/om/engine_impl.cxx
                      include/om/engine.hxx
                      include/om/game.hxx)
                                                 
set_target_properties(om PROPERTIES ENABLE_EXPORTS TRUE)
target_include_directories(om PRIVATE include)
target_link_libraries(om PRIVATE ${SDL2_LIB}
                                  stdc++fs)
target_compile_features(om PRIVATE cxx_std_17)

if(WIN32)
target_compile_definitions(om PRIVATE "-DOM_EXP=__declspec(dllexport)")
endif()

install(TARGETS om
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../bin)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
        
