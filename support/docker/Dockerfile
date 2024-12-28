FROM fedora:latest

RUN dnf update -y && dnf upgrade -y

# take this from https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies
RUN dnf install -y gcc \
    gcc-c++ \
    libstdc++-static \
    glibc-static \
    clang \
    git-core \
    wine \
    mingw64-gcc \
    mingw64-gcc-c++ \
    make \
    cmake \
    ninja-build \
    alsa-lib-devel \
    pulseaudio-libs-devel \
    nas-devel pipewire-devel \
    libX11-devel \
    libXext-devel \
    libXrandr-devel \
    libXcursor-devel \
    libXfixes-devel \
    libXi-devel \
    libXScrnSaver-devel \
    dbus-devel \
    ibus-devel \
    fcitx-devel \
    systemd-devel \
    mesa-libGL-devel \
    libxkbcommon-devel \
    mesa-libGLES-devel \
    mesa-libEGL-devel \
    vulkan-devel \
    wayland-devel \
    wayland-protocols-devel \
    libdrm-devel \
    mesa-libgbm-devel \
    libdecor-devel \
    pipewire-jack-audio-connection-kit-devel \
    liburing-devel
