cmake_minimum_required(VERSION 3.16)
project(02-sdl-dynamic)

add_executable(02-sdl-dynamic main.cxx)
target_compile_features(02-sdl-dynamic PUBLIC cxx_std_17)

# MacOSX - brew install sdl2
# MinGW  - pacman -s sdl2
# MSVC   - vcpkg install sdl2
# Fedora - dnf install sdl2-devel sdl2-static
# set(SDL2_DIR /usr/local/lib/cmake/SDL2) # uncomment for local SDL2 build
find_package(SDL2 REQUIRED)

if (UNIX)
    target_link_libraries(02-sdl-dynamic PRIVATE SDL2::SDL2 SDL2::SDL2main)
elseif (MINGW)
    # find out what libraries are needed for dynamicaly linking with libSDL.so
    # using mingw64 cross-compiler
    
    #$> /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --libs
    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main -lSDL2 -mwindows
    
    target_link_libraries(02-sdl-dynamic PRIVATE
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               )
elseif(MSVC)
    target_link_libraries(02-sdl-dynamic PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()

install(TARGETS 02-sdl-dynamic
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
