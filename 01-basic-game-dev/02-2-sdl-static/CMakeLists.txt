cmake_minimum_required(VERSION 3.20)
project(02-sdl-static)

add_executable(02-sdl-static main.cxx)
target_compile_features(02-sdl-static PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)
#target_include_directories(02-sdl-static PRIVATE ${SDL2_INCLUDE_DIRS})
# We want to link with our own debug version on SDL2 lib statically
# So before try it you have to build locally SDL2 and install it (usually to /usr/local)
#find_library(SDL2_LIB libSDL2d.a)
## if you need to link with our own compiled library you may point cmake to it.
#find_library(SDL2_LIB
#             NAMES libSDL2d.a
#             PATHS /usr/local /usr/local/lib /usr/local/lib
#             )
# if you just want to install system package with static version of SDL2 libs
# >sudo dnf install SDL2-static (on Fedora)

if(TARGET SDL3::SDL3-static)
    get_target_property(PATH_TO_SDL3_STATIC SDL3::SDL3-static LOCATION)
    message(STATUS "path to static libSDL3.a [${PATH_TO_SDL3_STATIC}]")
else()
    message(FATAL_ERROR "Error: find_library(...) did not find libSDL3.a")
endif()

if(MINGW)
    # find out what libraries are needed for statically linking with libSDL.a
    # using mingw64 cross-compiler

    #$> $ /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --static-libs
    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main
    #-lSDL2 -mwindows -Wl,--no-undefined -lm -ldinput8 -ldxguid -ldxerr8 -luser32
    #-lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid
    #-static-libgcc

    target_link_libraries(
        02-sdl-static
        PRIVATE
            SDL2::SDL2-static
            SDL2::SDL2main
            -lmingw32
            # old style ${SDL2_LIB} # full path to libSDL2d.a force to statically link with it
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
            -lsetupapi
            -static-libgcc)
elseif(UNIX)
    if(APPLE)
        # find out what libraries are needed for statically linking with libSDL.a on MacOS with HOMEBREW (currently clang++ not supported c++17)
        # $> /usr/local/bin/sdl2-config --static-libs
        # -L/usr/local/lib -lSDL2 -lm -liconv -Wl,-framework,CoreAudio
        # -Wl,-framework,AudioToolbox -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa
        # -Wl,-framework,Carbon -Wl,-framework,IOKit

        target_link_libraries(
            02-sdl-static
            PRIVATE
                "${SDL2_LIB}" # full path to libSDL2.a force to statically link with it
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
                -Wl,-weak_framework,QuartzCore
                -Wl,-weak_framework,Metal)
    else()
        # find out what libraries are needed for statically linking with libSDL.a on Linux
        # using default linux compiler
        # $> sdl2-config --static-libs
        # -lSDL2 -Wl,--no-undefined -lm -ldl -lpthread -lrt

        target_link_libraries(
            02-sdl-static
            PRIVATE
                SDL3::SDL3-static
                #-static # fully static build don't work on Fedora (you should recompile some packages yourself)
            )
        set_property(TARGET 02-sdl-static PROPERTY POSITION_INDEPENDENT_CODE
                                                   TRUE)
    endif()
elseif(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-static)
endif()
