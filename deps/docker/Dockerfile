# syntax=docker/dockerfile:1

# Docker image for building the OM project in CI (GitHub Actions) and locally.
#
# The base is ALT Linux p11 (the same branch as the maintainer's ALT 11.5
# workstation) plus the same toolchain versions used on that machine:
#   - GCC 13          (from p11; needed so clang/LLD find crt/libgcc paths,
#                      see cmake/toolchain/Linux.cmake)
#   - LLVM 23.1.2     (official release tarball, minimal set: clang, lld,
#                      libc++/libc++abi/libunwind, compiler-rt, `import std`
#                      module sources)
#   - ICU 70.1        (built into /opt/icu70; lld from the LLVM tarball links
#                      against libicu*.so.70, p11 only provides libicu74)
#   - CMake 4.5.1     (official binary, p11 has only 3.31 < required 4.5.1)
#   - Ninja 1.13.2    (official binary)
#   - Vulkan SDK      (slangc + glslangValidator + Vulkan headers/loader)
#
# All third-party project dependencies are built inside the image
# (deps/rules/linux-build.cmake) and land in /deps/linux-clang23-x86_64.
#
# Build from the repository root:
#   docker build -f deps/docker/Dockerfile \
#       -t ghcr.io/leanid/om-deps:linux-clang23 .
# See deps/docker/readme.md for usage and publishing instructions.

ARG ALT_TAG=p11

# ---------------------------------------------------------------------------
# toolchain: base system + compilers + SDKs (shared by both later stages)
# ---------------------------------------------------------------------------
FROM alt:${ALT_TAG} AS toolchain

ARG LLVM_VERSION=23.1.2
ARG CMAKE_VERSION=4.5.1
ARG NINJA_VERSION=1.13.2
ARG VULKAN_SDK_VERSION=1.4.335.0
ARG ICU_VERSION=70_1
ARG ICU_RELEASE=70-1

ENV PATH="/opt/cmake/bin:/opt/llvm/bin:/opt/vulkan/x86_64/bin:${PATH}"
ENV VULKAN_SDK="/opt/vulkan/x86_64"

# 1. System packages from ALT p11.
#    - gcc 13 + libstdc++-devel(-static): cmake/toolchain/Linux.cmake runs
#      `gcc -print-libgcc-file-name` to locate crt/libgcc paths for LLD.
#    - SDL3 build dependencies (see readme.md, "On Alt Linux"; JACK is
#      skipped, SDL3 builds fine without it).
#    - liblzma/zlib: runtime dependencies of the LLVM tarball binaries.
RUN apt-get update && apt-get install -y \
        gcc gcc-c++ libstdc++-devel libstdc++-devel-static binutils glibc-devel \
        git patch make python3 pkg-config \
        curl wget xz zstd tar unzip which ca-certificates \
        liblzma zlib zlib-devel libzstd libzstd-devel \
        libffi-devel libssl-devel \
        libpng-devel libjpeg-devel bzlib-devel \
        libwayland-client-devel libwayland-server-devel \
        libEGL-devel libGL-devel \
        libalsa-devel libpulseaudio-devel libsndio7-devel \
        libdrm-devel libgbm-devel \
        libwayland-egl-devel libwayland-cursor-devel libxkbcommon-devel \
        libX11-devel libXcursor-devel libXext-devel libXfixes-devel \
        libXtst-devel libXrandr-devel libXScrnSaver-devel \
        liburing-devel libunwind-devel libusb-devel libudev-devel libdecor-devel \
        wayland-devel wayland-protocols \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /var/cache/apt/*

# 2. ICU 70.1 -> /opt/icu70 (shared libs only).
RUN curl -fsSL https://github.com/unicode-org/icu/releases/download/release-${ICU_RELEASE}/icu4c-${ICU_VERSION}-src.tgz \
        | tar -xz -C /tmp \
    && mkdir /tmp/icu-build && cd /tmp/icu-build \
    && /tmp/icu/source/runConfigureICU Linux \
        --prefix=/opt/icu70 --libdir=/opt/icu70/lib64 \
        --enable-shared --disable-static --disable-samples --disable-tests \
    && make -j"$(nproc)" && make install \
    && find /opt/icu70 -mindepth 1 -maxdepth 1 ! -name 'lib64' -exec rm -rf {} + \
    && find /opt/icu70/lib64 -mindepth 1 -maxdepth 1 ! -name '*.so*' -exec rm -rf {} + \
    && rm -rf /tmp/icu /tmp/icu-build

# 3. LLVM -> /opt/llvm (minimal set; the full tarball unpacks to ~12 GB,
#    we keep only what the OM build needs).
#    Kept:
#      bin/    clang-23 (+clang, clang++ symlinks), lld (+ld.lld, ...)
#      lib/    clang/23 resource dir (headers, compiler-rt builtins),
#              x86_64-unknown-linux-gnu: libc++/libc++abi/libunwind
#              (.so and .a), libc++.modules.json
#      include c++ (libc++ headers), x86_64-unknown-linux-gnu/c++,
#              libunwind headers
#      share/  libc++/v1/*.cppm (sources for `import std`)
RUN curl -fsSL -o /tmp/llvm.tar.zst https://github.com/llvm/llvm-project/releases/download/llvmorg-${LLVM_VERSION}/LLVM-${LLVM_VERSION}-Linux-X64.tar.zst \
    && zstd -d --rm /tmp/llvm.tar.zst \
    && mkdir -p /opt/llvm \
    && tar -xf /tmp/llvm.tar -C /opt/llvm --strip-components=1 \
    && rm /tmp/llvm.tar \
    && find /opt/llvm/bin -mindepth 1 -maxdepth 1 \
        ! -name 'clang-23' ! -name 'clang' ! -name 'clang++' \
        ! -name 'lld' ! -name 'ld.lld' ! -name 'ld64.lld' ! -name 'wasm-ld' \
        -delete \
    && find /opt/llvm/lib -mindepth 1 -maxdepth 1 \
        ! -name 'clang' ! -name 'x86_64-unknown-linux-gnu' \
        -exec rm -rf {} + \
    && find /opt/llvm/lib/x86_64-unknown-linux-gnu -mindepth 1 -maxdepth 1 \
        \( -name 'libomp*' -o -name 'libarcher*' -o -name 'libompd*' \
           -o -name 'cmake' \) -exec rm -rf {} + \
    && find /opt/llvm/include -mindepth 1 -maxdepth 1 \
        ! -name 'c++' ! -name 'x86_64-unknown-linux-gnu' \
        ! -name 'unwind.h' ! -name 'unwind_*.h' \
        ! -name '__libunwind_config.h' ! -name 'libunwind*' \
        -exec rm -rf {} + \
    && find /opt/llvm/share -mindepth 1 -maxdepth 1 ! -name 'libc++' -exec rm -rf {} + \
    && rm -rf /opt/llvm/libexec

# 4. CMake + Ninja (official binaries; p11's cmake 3.31 is too old,
#    the project requires >= 4.5.1).
RUN curl -fsSL -o /tmp/cmake.tar.gz https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz \
    && mkdir -p /opt/cmake \
    && tar -xzf /tmp/cmake.tar.gz -C /opt/cmake --strip-components=1 \
    && rm /tmp/cmake.tar.gz \
    && curl -fsSL -o /tmp/ninja.zip https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/ninja-linux.zip \
    && unzip -o /tmp/ninja.zip -d /usr/local/bin && rm /tmp/ninja.zip

# 5. Vulkan SDK (slangc, glslangValidator, Vulkan headers + loader).
RUN curl -fsSL -o /tmp/vulkansdk.tar.xz https://sdk.lunarg.com/sdk/download/${VULKAN_SDK_VERSION}/linux/vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.xz \
    && mkdir -p /tmp/vulkansdk /opt/vulkan \
    && tar -xf /tmp/vulkansdk.tar.xz -C /tmp/vulkansdk \
    && mv /tmp/vulkansdk/*/x86_64 /opt/vulkan/x86_64 \
    && rm -rf /tmp/vulkansdk /tmp/vulkansdk.tar.xz

# 6. Clang config files: on ALT Linux clang cannot locate the GCC
#    installation by itself (crtbeginS.o, -lgcc, -lgcc_s) — on the host this
#    is fixed by cmake/toolchain/Linux.cmake (-B/-L flags); here we bake the
#    same flags into /etc/clang/*.cfg so every clang/clang++ invocation finds
#    crt/libgcc even without the toolchain file.
RUN gcc_lib_dir="$(dirname "$(gcc -print-libgcc-file-name)")" \
    && mkdir -p /etc/clang \
    && for cfg in \
        x86_64-unknown-linux-gnu-clang.cfg \
        x86_64-unknown-linux-gnu-clang++.cfg \
        clang.cfg clang++.cfg; do \
        printf -- '-B%s\n-L%s\n' "$gcc_lib_dir" "$gcc_lib_dir" \
            > "/etc/clang/$cfg"; \
    done

# 7. Linker search paths + smoke test: the image build fails here if the
#    toolchain (clang + LLD + ICU + libc++) cannot compile, link and run.
RUN printf '%s\n' \
        /opt/icu70/lib64 \
        /opt/llvm/lib/x86_64-unknown-linux-gnu \
        /opt/vulkan/x86_64/lib \
        > /etc/ld.so.conf.d/om-toolchain.conf && ldconfig \
    && clang++ --version && ldd /opt/llvm/bin/lld | grep -q 'libicuuc.so.70' \
    && cmake --version && ninja --version \
    && slangc -v && glslangValidator --version \
    && printf '#import std;\nint main() { std::print("toolchain ok\\n"); }\n' \
        > /tmp/toolchain_check.cxx \
    && clang++ -std=c++23 -stdlib=libc++ -fuse-ld=lld \
        /tmp/toolchain_check.cxx -o /tmp/toolchain_check \
    && /tmp/toolchain_check \
    && rm /tmp/toolchain_check.cxx /tmp/toolchain_check

# ---------------------------------------------------------------------------
# deps: build all project dependencies inside the image
# ---------------------------------------------------------------------------
FROM toolchain AS deps

WORKDIR /opt/om/deps/rules
COPY deps/rules /opt/om/deps/rules
COPY cmake /opt/om/cmake

# The build installs into <source>/../prebuilt/<triplet>, i.e.
# /opt/om/deps/prebuilt/linux-clang23-x86_64 (triplet comes from clang 23).
RUN CXX=clang++ cmake -P linux-build.cmake \
    && rm -rf /opt/om/deps/rules/build

# ---------------------------------------------------------------------------
# final: toolchain + pre-built dependencies
# ---------------------------------------------------------------------------
FROM toolchain AS final

ARG LLVM_VERSION
LABEL org.opencontainers.image.source="https://github.com/leanid/om" \
      org.opencontainers.image.description="ALT p11 + LLVM ${LLVM_VERSION} + pre-built OM dependencies (/deps/linux-clang23-x86_64) for CI builds"

COPY --from=deps /opt/om/deps/prebuilt/linux-clang23-x86_64 \
     /deps/linux-clang23-x86_64

# Some installed config files (.pc, some *-config.cmake) contain the absolute
# install prefix used at build time; rewrite it to the final location.
RUN old_prefix=/opt/om/deps/prebuilt/linux-clang23-x86_64 \
    && new_prefix=/deps/linux-clang23-x86_64 \
    && grep -rIl "$old_prefix" "$new_prefix" \
        | xargs -r sed -i "s|$old_prefix|$new_prefix|g"

# So `cmake --preset ninja-llvm` finds the dependencies out of the box.
ENV CMAKE_PREFIX_PATH="/deps/linux-clang23-x86_64" \
    OM_DEPS_DIR="/deps/linux-clang23-x86_64"

WORKDIR /src
CMD ["/bin/bash"]
