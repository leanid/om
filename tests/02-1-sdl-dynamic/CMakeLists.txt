cmake_minimum_required(VERSION 3.9)
project(02-sdl-dynamic)

add_executable(${PROJECT_NAME} main.cxx)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_library(SDL2_LIB libSDL2.so)

if (MINGW)
    # find out what libraries are needed for dynamicaly linking with libSDL.so
    # using mingw64 cross-compiler
    
    #$> /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --libs
    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main -lSDL2 -mwindows
    
    target_link_libraries(${PROJECT_NAME} 
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               )
elseif(UNIX)
    # find out what libraries are needed for dynamicaly linking with libSDL.so
    # using default linux compiler
    #$> sdl2-config --static-libs
    #-lSDL2
    
    target_link_libraries(${PROJECT_NAME} 
               -lSDL2
               )
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()
