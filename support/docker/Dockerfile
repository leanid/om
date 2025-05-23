FROM fedora:latest

RUN dnf update -y && dnf upgrade -y

# take this from https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies
# autoconf, automake, autoconf-archive - needs to build boost[icu] with vcpkg toolchain
# static linking is not supported or you need to install libcxx-devel libcxx-static for clang++
# or you need to install libc++-devel libc++-static for g++ sudo dnf install libstdc++-static glibc-static
RUN dnf install -y \
    alsa-lib-devel \
    autoconf \
    autoconf-archive \
    automake \
    clang \
    cmake \
    dbus-devel \
    fcitx-devel \
    gcc \
    gcc-c++ \
    git-core \
    glibc-static \
    ibus-devel \
    libX11-devel \
    libXScrnSaver-devel \
    libXcursor-devel \
    libXext-devel \
    libXfixes-devel \
    libXi-devel \
    libXrandr-devel \
    libcxx-devel \
    libcxx-static \
    libdecor-devel \
    libdrm-devel \
    libstdc++-static \
    libunwind-devel \
    liburing-devel
    libusb1-devel \
    libxkbcommon-devel \
    make \
    mesa-libEGL-devel \
    mesa-libGL-devel \
    mesa-libGLES-devel \
    mesa-libgbm-devel \
    mingw64-gcc \
    mingw64-gcc-c++ \
    nas-devel pipewire-devel \
    ninja-build \
    pipewire-jack-audio-connection-kit-devel \
    pulseaudio-libs-devel \
    systemd-devel \
    vulkan-devel \
    wayland-devel \
    wayland-protocols-devel \
    wine
