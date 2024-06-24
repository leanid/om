add_library(io_read_file OBJECT read_file.cxx read_file.hxx)
target_include_directories(io_read_file PUBLIC ${CMAKE_CURRENT_LIST_DIR})
add_library(om::io::read_file ALIAS io_read_file)

add_executable(read_file_test read_file_test.cxx)
target_link_libraries(read_file_test PRIVATE om::io::read_file
                                             Catch2::Catch2WithMain)
