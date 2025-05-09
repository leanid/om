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

#pragma pack(push, 4)
struct bind
{
    SDL_Keycode      key;
    std::string_view name;
    event            event_pressed;
    event            event_released;
};
#pragma pack(pop)

const std::array<bind, 8> keys{
    { { .key=SDLK_W, .name="up", .event_pressed=event::up_pressed, .event_released=event::up_released },
      { .key=SDLK_A, .name="left", .event_pressed=event::left_pressed, .event_released=event::left_released },
      { .key=SDLK_S, .name="down", .event_pressed=event::down_pressed, .event_released=event::down_released },
      { .key=SDLK_D, .name="right", .event_pressed=event::right_pressed, .event_released=event::right_released },
      { .key=SDLK_LCTRL,
        .name="button1",
        .event_pressed=event::button1_pressed,
        .event_released=event::button1_released },
      { .key=SDLK_SPACE,
        .name="button2",
        .event_pressed=event::button2_pressed,
        .event_released=event::button2_released },
      { .key=SDLK_ESCAPE, .name="select", .event_pressed=event::select_pressed, .event_released=event::select_released },
      { .key=SDLK_RETURN, .name="start", .event_pressed=event::start_pressed, .event_released=event::start_released } }
};

static bool check_input(const SDL_Event& e, const bind*& result)
{
    using namespace std;

    const auto it = std::ranges::find_if(keys,
                           
                            [&](const bind& b) { return b.key == e.key.key; });

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

        int compiled = SDL_VERSION;
        int linked   = SDL_GetVersion();

        if (compiled != linked)
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
        if (window != nullptr)
        {
            // We have to create renderer cause without it
            // Window not visible on Wayland video driver
            SDL_Renderer* renderer = SDL_CreateRenderer(window, "opengl");
            if (renderer == nullptr)
            {
                cerr << SDL_GetError() << endl;
                SDL_Quit();
                return "error see stderr";
            }
            SDL_RenderPresent(renderer);
        }
        return "";
    }
    /// pool event from input queue
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
