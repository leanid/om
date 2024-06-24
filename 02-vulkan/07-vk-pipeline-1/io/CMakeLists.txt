add_library(io_read_file OBJECT read_file.cxx read_file.hxx)
target_include_directories(io_read_file PUBLIC ${CMAKE_CURRENT_LIST_DIR})
add_library(om::io::read_file ALIAS io_read_file)
