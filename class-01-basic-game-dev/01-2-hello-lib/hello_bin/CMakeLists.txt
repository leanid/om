cmake_minimum_required(VERSION 3.16)
project(hello-bin CXX)

if (NOT MINGW)
    message("try to fix MinGW64 static linking error for F31 MinGW 6.0")
  
    add_executable(hello-bin-static src/main.cxx)
    target_compile_features(hello-bin-static PRIVATE cxx_std_17)
    target_link_libraries(hello-bin-static LINK_PUBLIC hello-lib-static)
    target_link_options(hello-bin-static PRIVATE -static)
endif()

if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)
endif()

add_executable(hello-bin-dynamic src/main.cxx)
target_compile_features(hello-bin-dynamic PRIVATE cxx_std_17)
target_link_libraries(hello-bin-dynamic LINK_PUBLIC hello-lib-dynamic)

install(TARGETS hello-bin-dynamic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../../bin/tests)
