#include "engine.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <SDL2/SDL.h>
#if __MINGW32__
#include <SDL2/SDL_opengl.h> // on windows for mingw
#else
#include <SDL2/SDL_opengles2.h>
#endif

#define OM_GL_CHECK()                                                          \
    {                                                                          \
        const int err = glGetError();                                          \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            switch (err)                                                       \
            {                                                                  \
                case GL_INVALID_ENUM:                                          \
                    std::cerr << GL_INVALID_ENUM << std::endl;                 \
                    break;                                                     \
                case GL_INVALID_VALUE:                                         \
                    std::cerr << GL_INVALID_VALUE << std::endl;                \
                    break;                                                     \
                case GL_INVALID_OPERATION:                                     \
                    std::cerr << GL_INVALID_OPERATION << std::endl;            \
                    break;                                                     \
                case GL_INVALID_FRAMEBUFFER_OPERATION:                         \
                    std::cerr << GL_INVALID_FRAMEBUFFER_OPERATION              \
                              << std::endl;                                    \
                    break;                                                     \
                case GL_OUT_OF_MEMORY:                                         \
                    std::cerr << GL_OUT_OF_MEMORY << std::endl;                \
                    break;                                                     \
            }                                                                  \
            assert(false);                                                     \
        }                                                                      \
    }

namespace om
{

static std::array<std::string_view, 17> event_names = {
    { /// input events
      "left_pressed", "left_released", "right_pressed", "right_released",
      "up_pressed", "up_released", "down_pressed", "down_released",
      "select_pressed", "select_released", "start_pressed", "start_released",
      "button1_pressed", "button1_released", "button2_pressed",
      "button2_released",
      /// virtual console events
      "turn_off" }
};

std::ostream& operator<<(std::ostream& stream, const event e)
{
    std::uint32_t value   = static_cast<std::uint32_t>(e);
    std::uint32_t minimal = static_cast<std::uint32_t>(event::left_pressed);
    std::uint32_t maximal = static_cast<std::uint32_t>(event::turn_off);
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

std::istream& operator>>(std::istream& is, vertex& v)
{
    is >> v.x;
    is >> v.y;
    return is;
}

std::istream& operator>>(std::istream& is, triangle& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

struct bind
{
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
      { SDLK_LCTRL, "button1", event::button1_pressed,
        event::button1_released },
      { SDLK_SPACE, "button2", event::button2_pressed,
        event::button2_released },
      { SDLK_ESCAPE, "select", event::select_pressed, event::select_released },
      { SDLK_RETURN, "start", event::start_pressed, event::start_released } }
};

static bool check_input(const SDL_Event& e, const bind*& result)
{
    using namespace std;

    const auto it = find_if(begin(keys), end(keys), [&](const bind& b) {
        return b.key == e.key.keysym.sym;
    });

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

        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);

        if (SDL_COMPILEDVERSION !=
            SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
        {
            serr << "warning: SDL2 compiled and linked version mismatch: "
                 << compiled << " " << linked << endl;
        }

        const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
        if (init_result != 0)
        {
            const char* err_message = SDL_GetError();
            serr << "error: failed call SDL_Init: " << err_message << endl;
            return serr.str();
        }

        window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 640, 480,
                                  ::SDL_WINDOW_OPENGL);

        if (window == nullptr)
        {
            const char* err_message = SDL_GetError();
            serr << "error: failed call SDL_CreateWindow: " << err_message
                 << endl;
            SDL_Quit();
            return serr.str();
        }

        SDL_GLContext gl_context = SDL_GL_CreateContext(window);
        SDL_assert(gl_context != nullptr);

        int gl_major_ver = 0;
        int result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
        SDL_assert(result == 0);
        int gl_minor_ver = 0;
        result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
        SDL_assert(result == 0);

        if (gl_major_ver <= 2 && gl_minor_ver < 1)
        {
            std::clog << "current context opengl version: " << gl_major_ver
                      << '.' << gl_minor_ver << '\n'
                      << "need openg version at least: 2.1\n"
                      << std::flush;
            throw std::runtime_error("opengl version too low");
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

            if (sdl_event.type == SDL_QUIT)
            {
                e = event::turn_off;
                return true;
            }
            else if (sdl_event.type == SDL_KEYDOWN)
            {
                if (check_input(sdl_event, binding))
                {
                    e = binding->event_pressed;
                    return true;
                }
            }
            else if (sdl_event.type == SDL_KEYUP)
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
    void render_triangle(const triangle&) final
    {
        glClearColor(0.f, 1.0, 0.f, 0.0f);
        OM_GL_CHECK();
        glClear(GL_COLOR_BUFFER_BIT);
        OM_GL_CHECK();
        // TODO continue...
    }
    void swap_buffers() final { SDL_GL_SwapWindow(window); }
    void uninitialize() final {}

private:
    SDL_Window* window = nullptr;
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
    if (already_exist == false)
    {
        throw std::runtime_error("engine not created");
    }
    if (nullptr == e)
    {
        throw std::runtime_error("e is nullptr");
    }
    delete e;
}

engine::~engine() {}

} // end namespace om