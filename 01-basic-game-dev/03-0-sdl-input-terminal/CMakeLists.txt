cmake_minimum_required(VERSION 3.20..3.22)
project(03-0-sdl-input-terminal)

add_executable(03-0-sdl-input-terminal main.cxx)
target_compile_features(03-0-sdl-input-terminal PUBLIC cxx_std_20)

find_package(SDL3 REQUIRED)

target_link_libraries(03-0-sdl-input-terminal PRIVATE SDL3::SDL3-shared)

# If you have problems with Window creation try play with SDL3/test
# Check X11 driver working on your system
# SDL_VIDEO_DRIVER=x11 ./test/testwm
# Check Wayland driver working on your system
# SDL_VIDEO_DRIVER=wayland ./test/testwm
# Then you can play same with your binary to find out which works best
