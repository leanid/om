#include "engine.hxx"

#include <algorithm>
#include <array>
#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <SDL3/SDL.h>

namespace om
{

static std::array<std::string_view, 17> event_names = {
    { /// input events
      "left_pressed",
      "left_released",
      "right_pressed",
      "right_released",
      "up_pressed",
      "up_released",
      "down_pressed",
      "down_released",
      "select_pressed",
      "select_released",
      "start_pressed",
      "start_released",
      "button1_pressed",
      "button1_released",
      "button2_pressed",
      "button2_released",
      /// virtual console events
      "turn_off" }
};

std::ostream& operator<<(std::ostream& stream, const event& e)
{
    auto value   = static_cast<std::uint32_t>(e);
    auto maximal = static_cast<std::uint32_t>(event::turn_off);
    if (value <= maximal)
    {
        stream << event_names[value];
        return stream;
    }
    else
    {
        throw std::runtime_error("too big event value");
    }
}

static std::ostream& operator<<(std::ostream& out, const SDL_Version& v)
{
    out << static_cast<int>(v.major) << '.';
    out << static_cast<int>(v.minor) << '.';
    out << static_cast<int>(v.patch);
    return out;
}

struct bind
{
    bind(SDL_Keycode      k,
         std::string_view s,
         event            pressed,
         event            released) noexcept
        : key(k)
        , name(s)
        , event_pressed(pressed)
        , event_released(released)
    {
    }

    SDL_Keycode                       key;
    [[maybe_unused]] std::string_view name;
    event                             event_pressed;
    event                             event_released;
};

const std::array<bind, 8> keys{
    { { SDLK_w, "up", event::up_pressed, event::up_released },
      { SDLK_a, "left", event::left_pressed, event::left_released },
      { SDLK_s, "down", event::down_pressed, event::down_released },
      { SDLK_d, "right", event::right_pressed, event::right_released },
      { SDLK_LCTRL,
        "button1",
        event::button1_pressed,
        event::button1_released },
      { SDLK_SPACE,
        "button2",
        event::button2_pressed,
        event::button2_released },
      { SDLK_ESCAPE, "select", event::select_pressed, event::select_released },
      { SDLK_RETURN, "start", event::start_pressed, event::start_released } }
};

static bool check_input(const SDL_Event& e, const bind*& result)
{
    using namespace std;

    const auto it =
        find_if(begin(keys),
                end(keys),
                [&](const bind& b) { return b.key == e.key.keysym.sym; });

    if (it != end(keys))
    {
        result = &(*it);
        return true;
    }
    return false;
}

class engine_impl final : public engine
{
public:
    /// create main window
    /// on success return empty string
    std::string initialize(std::string_view /*config*/) final
    {
        using namespace std;

        stringstream serr;

        SDL_Version compiled = { 0, 0, 0 };
        SDL_Version linked   = { 0, 0, 0 };

        SDL_VERSION(&compiled)
        SDL_GetVersion(&linked);

        if (SDL_COMPILEDVERSION !=
            SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
        {
            serr << "warning: SDL3 compiled and linked version mismatch: "
                 << compiled << " " << linked << endl;
        }

        const int init_result = SDL_Init(0);
        if (init_result != 0)
        {
            const char* err_message = SDL_GetError();
            serr << "error: failed call SDL_Init: " << err_message << endl;
            return serr.str();
        }
        /*
                SDL_Window* const window = SDL_CreateWindow("title",
                                                            SDL_WINDOWPOS_CENTERED,
                                                            SDL_WINDOWPOS_CENTERED,
                                                            640,
                                                            480,
                                                            ::SDL_WINDOW_OPENGL);

                if (window == nullptr)
                {
                    const char* err_message = SDL_GetError();
                    serr << "error: failed call SDL_CreateWindow: " <<
           err_message
                         << endl;
                    SDL_Quit();
                    return serr.str();
                }

                // Open the first available controller.
                SDL_GameController* controller = NULL;
                for (int i = 0; i < SDL_NumJoysticks(); ++i)
                {
                    if (SDL_IsGameController(i))
                    {
                        controller = SDL_GameControllerOpen(i);
                        if (controller)
                        {
                            break;
                        }
                        else
                        {
                            fprintf(stderr,
                                    "Could not open gamecontroller %i: %s\n",
                                    i,
                                    SDL_GetError());
                        }
                    }
                }
        */
        return "";
    }
    /// pool event from input queue
    /// return true if more events in queue
    bool read_input(event& e) final
    {
        using namespace std;
        // collect all events from SDL
        SDL_Event sdl_event;
        if (SDL_PollEvent(&sdl_event))
        {
            const bind* binding = nullptr;

            if (sdl_event.type == SDL_EVENT_QUIT)
            {
                e = event::turn_off;
                return true;
            }
            else if (sdl_event.type == SDL_EVENT_KEY_DOWN)
            {
                if (check_input(sdl_event, binding))
                {
                    e = binding->event_pressed;
                    return true;
                }
            }
            else if (sdl_event.type == SDL_EVENT_KEY_UP)
            {
                if (check_input(sdl_event, binding))
                {
                    e = binding->event_released;
                    return true;
                }
            }
            else if (sdl_event.type == SDL_EVENT_GAMEPAD_ADDED)
            {
                // TODO map controller to user
                std::cerr << "controller added" << std::endl;
                // continue with next event in queue
                return read_input(e);
            }
            else if (sdl_event.type == SDL_EVENT_GAMEPAD_REMOVED)
            {
                std::cerr << "controller removed" << std::endl;
            }
            else if (sdl_event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ||
                     sdl_event.type == SDL_EVENT_GAMEPAD_BUTTON_UP)
            {
                // TODO finish implementation
                if (sdl_event.button.state == SDL_PRESSED)
                {
                    e = event::button1_pressed;
                }
                else
                {
                    e = event::button1_released;
                }
                return true;
            }
        }
        return false;
    }
    void uninitialize() final {}
};

static bool already_exist = false;

engine* create_engine()
{
    if (already_exist)
    {
        throw std::runtime_error("engine already exist");
    }
    engine* result = new engine_impl();
    already_exist  = true;
    return result;
}

void destroy_engine(engine* e)
{
    if (already_exist)
    {
        if (nullptr == e)
        {
            throw std::runtime_error("e is nullptr");
        }
        delete e;
    }
    else
    {
        throw std::runtime_error("engine not created");
    }
}

engine::~engine() = default;

} // end namespace om

#ifdef _WIN32
#include <io.h>
#include <windows.h>
void fix_windows_console();
#endif

om::game* reload_game(om::game*   old,
                      const char* library_name,
                      const char* tmp_library_name,
                      om::engine& engine,
                      void*&      old_handle);

// clang-format off
#ifdef __cplusplus
extern "C"
#endif
int main(int /*argc*/, char* /*argv*/[])
// clang-format on
{
    std::unique_ptr<om::engine, void (*)(om::engine*)> engine(
        om::create_engine(), om::destroy_engine);

    std::string err = engine->initialize("");
    if (!err.empty())
    {
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "start app\n" << std::endl;

    if (!std::cout)
    {
#ifdef _WIN32
        fix_windows_console();
#endif
    }

    using namespace std::string_literals;
    // mingw library name for windows
    const char* library_name = SDL_GetPlatform() == "Windows"s
                                   ? "libgame-03-4.dll"
                                   : "./libgame-03-4.so";

    using namespace std::filesystem;

    const char* tmp_library_file = "./temp.dll";

    void*     game_library_handle{};
    om::game* game = reload_game(
        nullptr, library_name, tmp_library_file, *engine, game_library_handle);

    auto time_during_loading = last_write_time(library_name);

    game->initialize();

    bool continue_loop = true;
    while (continue_loop)
    {
        auto current_write_time = last_write_time(library_name);

        if (current_write_time != time_during_loading)
        {
            file_time_type next_write_time;
            // wait while library file finish to changing
            for (;;)
            {
                using namespace std::chrono;
                std::this_thread::sleep_for(milliseconds(100));
                next_write_time = last_write_time(library_name);
                if (next_write_time != current_write_time)
                {
                    current_write_time = next_write_time;
                }
                else
                {
                    break;
                }
            }

            std::cout << "reloading game" << std::endl;
            game = reload_game(game,
                               library_name,
                               tmp_library_file,
                               *engine,
                               game_library_handle);

            if (game == nullptr)
            {
                std::cerr << "next attempt to reload game..." << std::endl;
                continue;
            }

            time_during_loading = next_write_time;
        }

        om::event event;

        while (engine->read_input(event))
        {
            std::cout << event << std::endl;
            if (event == om::event::turn_off)
            {
                continue_loop = false;
                break;
            }
            else
            {
                game->on_event(event);
            }
        }

        game->update();
        game->render();
    }

    engine->uninitialize();

    return EXIT_SUCCESS;
}

om::game* reload_game(om::game*   old,
                      const char* library_name,
                      const char* tmp_library_name,
                      om::engine& engine,
                      void*&      old_handle)
{
    using namespace std::filesystem;

    if (old)
    {
        SDL_UnloadObject(old_handle);
    }

    if (std::filesystem::exists(tmp_library_name))
    {
        if (0 != remove(tmp_library_name))
        {
            std::cerr << "error: can't remove: " << tmp_library_name
                      << std::endl;
            return nullptr;
        }
    }

    try
    {
        copy(library_name, tmp_library_name); // throw on error
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error: can't copy [" << library_name << "] to ["
                  << tmp_library_name << "]" << std::endl;
        return nullptr;
    }

    void* game_handle = SDL_LoadObject(tmp_library_name);

    if (game_handle == nullptr)
    {
        std::cerr << "error: failed to load: [" << tmp_library_name << "] "
                  << SDL_GetError() << std::endl;
        return nullptr;
    }

    old_handle = game_handle;

    SDL_FunctionPointer create_game_func_ptr =
        SDL_LoadFunction(game_handle, "create_game");

    if (create_game_func_ptr == nullptr)
    {
        std::cerr << "error: no function [create_game] in " << tmp_library_name
                  << " " << SDL_GetError() << std::endl;
        return nullptr;
    }
    // void* destroy_game_func_ptr = SDL_LoadFunction(game_handle,
    // "destroy_game");

    using create_game_ptr = decltype(&create_game);

    auto create_game_func =
        reinterpret_cast<create_game_ptr>(create_game_func_ptr);

    om::game* game = create_game_func(&engine);

    if (game == nullptr)
    {
        std::cerr << "error: func [create_game] returned: nullptr" << std::endl;
        return nullptr;
    }
    return game;
}

#ifdef _WIN32
void fix_windows_console()
{
    const BOOL result = AttachConsole(ATTACH_PARENT_PROCESS);
    if (!result)
    {
        if (!AllocConsole())
        {
            throw std::runtime_error("can't allocate console");
        }
    }

    FILE* f = std::freopen("CON", "w", stdout);
    if (!f)
    {
        throw std::runtime_error("can't reopen stdout");
    }
    FILE* fe = std::freopen("CON", "w", stderr);
    if (!fe)
    {
        throw std::runtime_error("can't reopen stderr");
    }
    std::cout.clear();
    std::cout << " \b";
    std::cerr << " \b";

    if (!std::cout.good() || !std::cerr.good())
    {
        throw std::runtime_error("can't print with std::cout");
    }
}
#endif
