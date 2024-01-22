#include "engine.hxx"

#include <algorithm>
#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

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
    auto minimal = static_cast<std::uint32_t>(event::left_pressed);
    auto maximal = static_cast<std::uint32_t>(event::turn_off);
    if (value >= minimal && value <= maximal)
    {
        stream << event_names[value];
        return stream;
    }
    else
    {
        throw std::runtime_error("too big event value");
    }
}

static std::ostream& operator<<(std::ostream& out, const SDL_version& v)
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

    SDL_Keycode      key;
    std::string_view name;
    event            event_pressed;
    event            event_released;
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

        SDL_version compiled = { 0, 0, 0 };
        SDL_version linked   = { 0, 0, 0 };

        SDL_VERSION(&compiled)
        SDL_GetVersion(&linked);

        if (SDL_COMPILEDVERSION !=
            SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
        {
            serr << "warning: SDL2 compiled and linked version mismatch: "
                 << compiled << " " << linked << endl;
        }

        const int init_result = SDL_Init(SDL_INIT_VIDEO);
        if (init_result != 0)
        {
            const char* err_message = SDL_GetError();
            serr << "error: failed call SDL_Init: " << err_message << endl;
            return serr.str();
        }

        SDL_Window* const window =
            SDL_CreateWindow("title", 640, 480, SDL_WINDOW_OPENGL);

        if (window == nullptr)
        {
            const char* err_message = SDL_GetError();
            serr << "error: failed call SDL_CreateWindow: " << err_message
                 << endl;
            SDL_Quit();
            return serr.str();
        }
        // We have to create renderer cause without it
        // Window not visible on Wayland video driver
        SDL_Renderer* renderer =
            SDL_CreateRenderer(window, "opengl", SDL_RENDERER_ACCELERATED);
        if (renderer == nullptr)
        {
            cerr << SDL_GetError() << endl;
            SDL_Quit();
            return "error see stderr";
        }
        SDL_RenderPresent(renderer);
        /* Open the first available controller. */
        int             num_connected_gamepads = 0;
        SDL_Gamepad*    controller             = nullptr;
        SDL_JoystickID* joysticks = SDL_GetGamepads(&num_connected_gamepads);
        for (int i = 0; i < num_connected_gamepads; ++i)
        {
            if (SDL_IsGamepad(i))
            {
                controller = SDL_OpenGamepad(i);
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
    if (!already_exist)
    {
        throw std::runtime_error("engine not created");
    }
    if (nullptr == e)
    {
        throw std::runtime_error("e is nullptr");
    }
    delete e;
}

engine::~engine() = default;

} // end namespace om
