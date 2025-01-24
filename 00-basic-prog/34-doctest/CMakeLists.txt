cmake_minimum_required(VERSION 3.31)

project(34-doctest CXX)

add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${CMAKE_CURRENT_LIST_DIR})

include(FetchContent)
fetchcontent_declare(
    trompeloeil
    URL https://github.com/rollbear/trompeloeil/archive/refs/tags/v49.tar.gz)
fetchcontent_makeavailable(trompeloeil)

add_executable(34-doctest main.cxx)
target_link_libraries(34-doctest PRIVATE doctest)

add_executable(34-doctest-trompeloeil main_moc.cxx)
target_link_libraries(34-doctest-trompeloeil PRIVATE doctest
                                                     trompeloeil::trompeloeil)

enable_testing()

add_test(NAME 34-doctest COMMAND 34-doctest)
add_test(NAME 34-doctest-trompeloeil COMMAND 34-doctest-trompeloeil)
