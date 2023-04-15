cmake_minimum_required(VERSION 3.19.7...3.22)
project(02-sdl-dynamic)

add_executable(02-sdl-dynamic main.cxx)
target_compile_features(02-sdl-dynamic PUBLIC cxx_std_20)

# MacOSX - brew install sdl3
# MinGW  - pacman -s sdl3
# MSVC   - vcpkg install sdl3
# Fedora - dnf install sdl3-devel sdl3-static
# set(SDL3_DIR /usr/local/lib64/cmake/SDL3) # uncomment for local SDL3 build
# -----------------
# Or build yourself fresh SDL3 and install it.
# On Fedora better use `sudo dnf install pipewire-devel` and SDL_PIPEWIRE=ON SDL_PIPEWIRE_SHARED=ON
# To test audio ./test/testaudioinfo - checkout all audio drivers and output devices
# To test pipewire `SDL_AUDIO_DRIVER=PIPEWIRE ./test/testaudioinfo`
# To test Window Manager `./test/testwm --video X11` or `./test/testwm --video wayland`
# -----------------
# Also on Fedora I installed: libxkbcommon-devel, libdrm-devel, pulseaudio-libs-devel, pipewire-devel
find_package(SDL3 REQUIRED)

if(MINGW)
    # find out what libraries are needed for dynamically linking with libSDL.so
    # using mingw64 cross-compiler

    #$> /usr/x86_64-w64-mingw32/sys-root/mingw/bin/sdl2-config --libs
    #-L/usr/x86_64-w64-mingw32/sys-root/mingw/lib -lmingw32 -lSDL2main -lSDL2 -mwindows
    target_link_libraries(
        02-sdl-dynamic
        PRIVATE -lmingw32
                -lSDL2main
                -lSDL2
                -mwindows)
else()
    target_link_libraries(02-sdl-dynamic PRIVATE SDL3::SDL3-shared)
endif()
