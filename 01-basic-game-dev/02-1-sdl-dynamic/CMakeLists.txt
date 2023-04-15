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
# Full list of all packages I install on Fedora 37 is:
# `sudo yum install gcc git-core make cmake autoconf automake libtool alsa-lib-devel \
#  pulseaudio-libs-devel nas-devel pipewire-devel libX11-devel libXext-devel \
#  libXrandr-devel libXcursor-devel libXfixes-devel libXi-devel libXScrnSaver-devel \
#  dbus-devel ibus-devel fcitx-devel systemd-devel mesa-libGL-devel libxkbcommon-devel \
#  mesa-libGLES-devel mesa-libEGL-devel vulkan-devel wayland-devel wayland-protocols-devel \
#  libdrm-devel mesa-libgbm-devel libusb1-devel libdecor-devel libsamplerate-devel \
#  pipewire-jack-audio-connection-kit-devel`
find_package(SDL3 REQUIRED)

target_link_libraries(02-sdl-dynamic PRIVATE SDL3::SDL3-shared)
