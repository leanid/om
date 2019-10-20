cmake_minimum_required(VERSION 3.14)
project(hello-bin CXX)

add_executable(hello-bin-static src/main.cxx)
target_compile_features(hello-bin-static PRIVATE cxx_std_17)
target_link_libraries(hello-bin-static LINK_PUBLIC hello-lib-static)
target_link_options(hello-bin-static PRIVATE -static)

add_executable(hello-bin-dynamic src/main.cxx)
target_compile_features(hello-bin-dynamic PRIVATE cxx_std_17)
target_link_libraries(hello-bin-dynamic LINK_PUBLIC hello-lib-dynamic)

install(TARGETS hello-bin-static hello-bin-dynamic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests)
