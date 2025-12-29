#include "experimental/scope"

import std;
import log;
import vulkan_args_parser;
import vulkan_platform_sdl3;
import vulkan_render;
import vulkan;
import sdl.SDL;
import sdl.vulkan;
import glm;

int main_cant_throw(int argc, char** argv);

/// always use alignas to be shure C++ and Slang
/// structs has same alignment spec:
/// https://docs.vulkan.org/spec/latest/chapters/interfaces.html#interfaces-resources-layout
struct uniform_buffer_object
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

int main(int argc, char** argv)
{
    try
    {
        return main_cant_throw(argc, argv);
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        std::terminate();
    }
    return 1;
}

int main_cant_throw(int argc, char** argv)
{
    using namespace om;

    std::ios_base::sync_with_stdio(false); // faster iostream work and we don't
                                           // need to sync with cstdio

    const char*   env_log_file = std::getenv("OM_LOG");
    std::ofstream log_file;
    if (env_log_file)
    {
        log_file.open(env_log_file, std::ios::binary);
        if (!log_file.is_open())
        {
            std::cerr << "error: can't open log file from ${ENV}OM_LOG="
                      << env_log_file << std::endl;
        }
        else
        {
            std::clog.rdbuf(log_file.rdbuf());
        }
    }
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

    om::cout << args_parser << '\n';

    if (vk_validation_layer)
    {
        om::cout << "enable vulkan validation layers\n";
        if (!sdl::SetHint("SDL_RENDER_VULKAN_DEBUG", "1"))
        {
            om::cout << sdl::GetError();
            return 1;
        }
    }

    if (!sdl::Init(sdl::InitFlags::VIDEO))
    {
        om::cout << sdl::GetError();
        return 1;
    }
    om::cout << "sdl init Video\n";
    std::experimental::scope_exit quit(
        []()
        {
            sdl::Quit();
            om::cout << "sdl destroy Video\n";
        });

    if (!sdl::vulkan::Vulkan_LoadLibrary(nullptr))
    {
        om::cout << "error: failed to load Vulkan library: " << sdl::GetError()
                 << '\n';
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
        };
        std::vector<std::uint16_t> mesh_indexes = {
            0, 1, 2, 2, 3, 0
        };
        // clang-format on
        uniform_buffer_object ubo{};

        om::vulkan::mesh mesh(std::span{ mesh_verticles },
                              std::span{ mesh_indexes },
                              render,
                              "rect");

        auto startTime = std::chrono::high_resolution_clock::now();

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

            auto  currentTime = std::chrono::high_resolution_clock::now();
            float time =
                std::chrono::duration<float, std::chrono::seconds::period>(
                    currentTime - startTime)
                    .count();

            auto window_size = render.get_swapchain_image_extent();

            ubo.model = glm::rotate(glm::mat4(1.0f),              // matrix
                                    time * glm::radians(90.0f),   // angle
                                    glm::vec3(0.0f, 0.0f, 1.0f)); // axis

            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),  // eye
                                   glm::vec3(0.0f, 0.0f, 0.0f),  // center
                                   glm::vec3(0.0f, 0.0f, 1.0f)); // up

            ubo.proj =
                glm::perspective(glm::radians(45.0f),
                                 static_cast<float>(window_size.width) /
                                     static_cast<float>(window_size.height),
                                 0.1f,
                                 10.0f);

            ubo.proj[1][1] *= -1; // in Vulkan y-asix point down

            render.draw(mesh,
                        std::span<std::byte>(reinterpret_cast<std::byte*>(&ubo),
                                             sizeof(ubo)));

            // running = false;
            // std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        render.wait_idle();
    }
    catch (const vk::SystemError& ex)
    {
        om::cout << "error: vk::SystemError what: [" << ex.what() << ']'
                 << "\n    code: [" << ex.code().message() << "]" << std::endl;
        std::terminate();
    }
    catch (const std::exception& ex)
    {
        om::cout << "error: got exception [" << ex.what() << ']' << std::endl;
    }

    return om::cout.fail() || om::cout.fail() ? 1 : 0;
}
