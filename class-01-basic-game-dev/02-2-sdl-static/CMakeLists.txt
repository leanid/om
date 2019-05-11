cmake_minimum_required(VERSION 3.9)
project(02-sdl-static)

add_executable(${PROJECT_NAME} main.cxx)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_library(SDL2_LIB NAMES libSDL2.a)

if(NOT SDL2_LIB)
    message("Error: find_library(...) did not find libSDL2.a")
endif()

if (MINGW)
    # find out what libraries are needed for staticaly linking with libSDL.a
    # using mingw64 cross-compiler
    
    #$> $ /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --static-libs
    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main 
    #-lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32 
    #-lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid 
    #-static-libgcc
    
    target_link_libraries(${PROJECT_NAME} 
               #SDL2::SDL2-static 
               #SDL2::SDL2main
               -lmingw32 
               -lSDL2main 
               ${SDL2_LIB} # full path to libSDL2.a force to staticaly link with it
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
    if(APPLE)
      # find out what libraries are needed for staticcaly linking with libSDL.a on MacOS with HOMEBREW (currently clang++ not supported c++17)
      # $> /usr/local/bin/sdl2-config --static-libs
      # -L/usr/local/lib -lSDL2 -lm -liconv -Wl,-framework,CoreAudio 
      # -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa 
      # -Wl,-framework,Carbon -Wl,-framework,IOKit

      target_link_libraries(${PROJECT_NAME} 
               "${SDL2_LIB}" # full path to libSDL2.a force to staticaly link with it
               -lm 
               -liconv 
               -Wl,-framework,CoreAudio 
               -Wl,-framework,AudioToolbox 
               -Wl,-framework,ForceFeedback 
               -lobjc 
               -Wl,-framework,CoreVideo 
               -Wl,-framework,Cocoa 
               -Wl,-framework,Carbon 
               -Wl,-framework,IOKit
               )
    else()
      # find out what libraries are needed for staticaly linking with libSDL.a on Linux
      # using default linux compiler
      # $> sdl2-config --static-libs
      # -lSDL2 -Wl,--no-undefined -lm -ldl -lpthread -lrt
    
      target_link_libraries(${PROJECT_NAME} 
               ${SDL2_LIB} # full path to libSDL2.a force to staticaly link with it
               # -Wl,--no-undefined # not compiled on MacOS
               -lm
               -ldl
               -lpthread
               -lrt
               )
    endif()
elseif(MSVC)
    find_package(sdl2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests
        ARCHIVE DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../bin/tests)
