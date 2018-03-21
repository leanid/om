cmake_minimum_required(VERSION 3.9)
project(03-sdl-loop)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

add_executable(${PROJECT_NAME} main.cxx)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_library(SDL2_LIB libSDL2.a 
    HINTS "C:/mingw-w64/x86_64-7.2.0-posix-seh-rt_v5-rev1/mingw64/x86_64-w64-mingw32/lib")

if (MINGW)
    # find out what libraries are needed for staticaly linking with libSDL.a
    # using mingw64 cross-compiler
    
    #$> $ /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --static-libs
    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main 
    #-lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 
    #-lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid 
    #-static-libgcc
    
    target_link_libraries(${PROJECT_NAME} 
               -lmingw32 
               -lSDL2main 
               "${SDL2_LIB}" # full path to libSDL2.a force to staticaly link with it
               -mwindows
               -Wl,--no-undefined
               -lm
               -ldinput8
               -ldxguid
               -ldxerr8
               -luser32 
               -lgdi32
               -lwinmm
               -limm32
               -lole32
               -loleaut32
               -lshell32
               -lversion
               -luuid
               -static-libgcc
               )
elseif(UNIX)
    # find out what libraries are needed for staticaly linking with libSDL.a
    # using default linux compiler
    #$> sdl2-config --static-libs
    # -lSDL2 -Wl,--no-undefined -lm -ldl -lpthread -lrt
    
    target_link_libraries(${PROJECT_NAME} 
               "${SDL2_LIB}" # full path to libSDL2.a force to staticaly link with it
               -lm
               -ldl
               -lpthread
               -lrt
               )
endif()


