#include "experimental/scope"

import std;
import log;
import vulkan_args_parser;
import vulkan_platform_sdl3;
import vulkan_render;
import vulkan_hpp;
import sdl.SDL;
import sdl.vulkan;

int main(int argc, char** argv)
{
    using namespace om;

    std::ios_base::sync_with_stdio(false); // faster iostream work and we don't
                                           // need to sync with cstdio

    vulkan::args_parser args_parser(argc, argv);

    if (!args_parser.help.empty())
    {

        std::cout << "usage: " << args_parser.help << '\n';
        return 0;
    }

    bool verbose               = args_parser.verbose;
    bool vk_validation_layer   = args_parser.validation_layer;
    bool vk_debug_callback_ext = args_parser.debug_callback;

    if (verbose)
    {
        om::cout.rdbuf(std::clog.rdbuf());
    }

    if (vk_validation_layer)
    {
        om::cout << "enable vulkan validation layers\n";
        if (!sdl::SetHint("SDL_RENDER_VULKAN_DEBUG", "1"))
        {
            std::cerr << sdl::GetError();
            return 1;
        }
    }

    if (!sdl::Init(sdl::InitFlags::VIDEO))
    {
        std::cerr << sdl::GetError();
        return 1;
    }
    om::cout << "create all subsystems\n";
    std::experimental::scope_exit quit(
        []()
        {
            sdl::Quit();
            om::cout << "destroy all subsystems\n";
        });

    if (!sdl::vulkan::Vulkan_LoadLibrary(nullptr))
    {
        std::cerr << sdl::GetError();
        return 1;
    }
    om::cout << "load vulkan library\n";
    std::experimental::scope_exit unload(
        []()
        {
            sdl::vulkan::Vulkan_UnloadLibrary();
            om::cout << "unload vulkan library\n";
        });

    sdl::WindowFlags window_flags =
        sdl::WindowFlags::VULKAN | sdl::WindowFlags::RESIZABLE;
    if (args_parser.high_pixel_density)
    {
        window_flags |= sdl::WindowFlags::HIGH_PIXEL_DENSITY;
    }

    std::unique_ptr<sdl::SDL_Window, decltype(&sdl::DestroyWindow)> window(
        sdl::CreateWindow("09-vk-res-loading-1", 800, 600, window_flags),
        sdl::DestroyWindow);
    std::experimental::scope_exit destroy_window(
        []() { om::cout << "destroy sdl window\n"; });

    if (!window)
    {
        om::cout << "error: can't create sdl window: " << sdl::GetError()
                 << std::endl;
        return 1;
    }

    om::cout << "sdl windows created\n";

    try
    {
        using namespace om::vulkan;
        platform_sdl3 platform(window.get(), om::cout);
        render::hints hints{
            .vulkan_version = { .major = args_parser.vulkan_version_major,
                                .minor = args_parser.vulkan_version_minor },
            .verbose        = verbose,
            .enable_validation_layers  = vk_validation_layer,
            .enable_debug_callback_ext = vk_debug_callback_ext
        };
        render render(platform, hints);

        // clang-format off
        std::vector<vertex> mesh_verticles = {
            {.pos{ 0.4f, -0.4f, 0.0f }, .col{1.0f, 0.0f, 0.0f}},
            {.pos{ 0.4f, 0.4f, 0.0f },  .col{0.0f, 1.0f, 0.0f}},
            {.pos{ -0.4f, 0.4f, 0.0f }, .col{0.0f, 0.0f, 1.0f}},

            {.pos{ -0.4f, -0.4f, 0.0f}, .col{1.0f, 1.0f, 0.0f}},
            {.pos{ 0.4f, -0.4f, 0.0f }, .col{1.0f, 0.0f, 0.0f}},
            {.pos{ -0.4f, 0.4f, 0.0f }, .col{0.0f, 0.0f, 1.0f}}
        };
        // clang-format on

        om::vulkan::mesh mesh(std::span{ mesh_verticles }, render, "rect");

        bool running = true;
        while (running)
        {
            sdl::Event event;
            while (sdl::PollEvent(&event))
            {
                switch (static_cast<sdl::EventType>(event.type)) // NOLINT
                {
                    case sdl::EventType::QUIT:
                        running = false;
                        break;
                    case sdl::EventType::KEY_DOWN:
                        if (static_cast<sdl::Keycode>(event.key.key) ==
                            sdl::Keycode::ESCAPE)
                            running = false;
                        break;
                    case sdl::EventType::WINDOW_RESIZED:
                        render.recreate_swapchain();
                    default:
                        break;
                }
            }

            render.draw(mesh);

            // running = false;
            // std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        render.wait_idle();
    }
    catch (const vk::SystemError& ex)
    {
        std::cerr << "error: vk::SystemError what: [" << ex.what() << ']'
                  << "\n    code: [" << ex.code().message() << "]" << std::endl;
        std::terminate();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: got exception [" << ex.what() << ']' << std::endl;
    }

    return std::cerr.fail() || std::cout.fail() ? 1 : 0;
}
