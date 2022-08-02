cmake_minimum_required(VERSION 3.19...3.23)
project(engine LANGUAGES CXX)

if(MSVC)
  #   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_library(scanner SHARED src/fs_scanner.cxx src/fs_scanner.hxx)
target_compile_features(scanner PRIVATE cxx_std_20)

add_executable(scanner_test src/fs_scanner_unit_test.cxx)
target_link_libraries(scanner_test scanner)
target_compile_features(scanner_test PRIVATE cxx_std_20)

add_executable(
  om
  include/om/engine.hxx
  include/om/game.hxx
  src/om/engine_impl.cxx
  src/om/main.cxx
)

set_target_properties(om PROPERTIES ENABLE_EXPORTS TRUE)
target_include_directories(om PUBLIC include)
target_link_libraries(om PRIVATE SDL2::SDL2 SDL2::SDL2main fmt::fmt)
target_compile_features(om PRIVATE cxx_std_20)

set_target_properties(
  om PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:om>"
  VS_DEBUGGER_COMMAND "$<TARGET_FILE:om>"
  VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;$<TARGET_FILE_DIR:game>;$<TARGET_FILE_DIR:SDL2::SDL2>;"
)

if(WIN32)
  target_compile_definitions(om PRIVATE "-DOM_EXP=__declspec(dllexport)")
endif()
