FROM fedora:latest

RUN dnf update -y && dnf upgrade -y

# take this from https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies
RUN dnf install -y \
    alsa-lib-devel \
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
    libdecor-devel \
    libdrm-devel \
    libstdc++-static \
    liburing-devel
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
