FROM fedora:latest

RUN dnf update -y && dnf upgrade -y

RUN dnf install -y gcc-c++ \
    make \
    cmake \
    mingw64-gcc \
    mingw64-gcc-c++ \
    clang \
    wine \
    git \
    SDL2-devel \
    SDL2-static \
    mingw64-SDL2 \
    mingw64-SDL2-static \
    libstdc++-static \
    glibc-static \
    ninja-build \
    pipewire-devel \
    libXext-devel

RUN git clone https://github.com/libsdl-org/SDL.git && \
    mkdir SDL/build && \
    cmake -S SDL -B SDL/build -DSDL_STATIC=ON && \
    cmake --build SDL/build && \
    cmake --install SDL/build
