#include "engine.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <SDL3/SDL_stdinc.h>

// we have to load all extension GL function pointers
// dynamically from opengl library
// so first declare function pointers for all we need
PFNGLCREATESHADERPROC            glCreateShader            = nullptr;
PFNGLSHADERSOURCEPROC            glShaderSource            = nullptr;
PFNGLCOMPILESHADERPROC           glCompileShader           = nullptr;
PFNGLGETSHADERIVPROC             glGetShaderiv             = nullptr;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog        = nullptr;
PFNGLDELETESHADERPROC            glDeleteShader            = nullptr;
PFNGLCREATEPROGRAMPROC           glCreateProgram           = nullptr;
PFNGLATTACHSHADERPROC            glAttachShader            = nullptr;
PFNGLBINDATTRIBLOCATIONPROC      glBindAttribLocation      = nullptr;
PFNGLLINKPROGRAMPROC             glLinkProgram             = nullptr;
PFNGLGETPROGRAMIVPROC            glGetProgramiv            = nullptr;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog       = nullptr;
PFNGLDELETEPROGRAMPROC           glDeleteProgram           = nullptr;
PFNGLUSEPROGRAMPROC              glUseProgram              = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer     = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVALIDATEPROGRAMPROC         glValidateProgram         = nullptr;

template <typename T> static void load_gl_func(const char* func_name, T& result)
{
    SDL_FunctionPointer gl_pointer = SDL_GL_GetProcAddress(func_name);
    if (nullptr == gl_pointer)
    {
        throw std::runtime_error(std::string("can't load GL function") +
                                 func_name);
    }
    result = reinterpret_cast<T>(gl_pointer);
}

#define OM_GL_CHECK()                                                          \
    {                                                                          \
        const GLenum err = glGetError();                                       \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            switch (err)                                                       \
            {                                                                  \
                case GL_INVALID_ENUM:                                          \
                    std::cerr << "GL_INVALID_ENUM" << std::endl;               \
                    break;                                                     \
                case GL_INVALID_VALUE:                                         \
                    std::cerr << "GL_INVALID_VALUE" << std::endl;              \
                    break;                                                     \
                case GL_INVALID_OPERATION:                                     \
                    std::cerr << "GL_INVALID_OPERATION" << std::endl;          \
                    break;                                                     \
                case GL_INVALID_FRAMEBUFFER_OPERATION:                         \
                    std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION"            \
                              << std::endl;                                    \
                    break;                                                     \
                case GL_OUT_OF_MEMORY:                                         \
                    std::cerr << "GL_OUT_OF_MEMORY" << std::endl;              \
                    break;                                                     \
                default:                                                       \
                    std::cerr << "unknown error" << std::endl;                 \
                    break;                                                     \
            }                                                                  \
            std::cerr << __FILE__ << ':' << __LINE__ << '(' << __FUNCTION__    \
                      << ')' << std::endl;                                     \
            assert(false);                                                     \
        }                                                                      \
    }

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

std::ostream& operator<<(std::ostream& stream, const event e)
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

std::istream& operator>>(std::istream& is, vertex& v)
{
    is >> v.x;
    is >> v.y;
    is >> v.z;
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
{ // NOLINTNEXTLINE
    bind(SDL_Keycode k, std::string_view s, event pressed, event released)
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
    { { SDLK_W, "up", event::up_pressed, event::up_released },
      { SDLK_A, "left", event::left_pressed, event::left_released },
      { SDLK_S, "down", event::down_pressed, event::down_released },
      { SDLK_D, "right", event::right_pressed, event::right_released },
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
        std::ranges::find_if(keys,

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
    std::string initialize(std::string_view /*config*/) final;
    /// pool event from input queue
    /// return true if more events in queue
    bool read_input(event& e) final;
    void render_triangle(const triangle& t) final;
    void swap_buffers() final;
    void uninitialize() final;

private:
    SDL_Window*   window      = nullptr;
    SDL_GLContext gl_context  = nullptr;
    GLuint        program_id_ = 0;
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

engine::~engine() = default;

std::string engine_impl::initialize(std::string_view)
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

    const bool init_result = SDL_Init(SDL_INIT_VIDEO);
    if (!init_result)
    {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_Init: " << err_message << endl;
        return serr.str();
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // On Apple's OS X you must set the NSHighResolutionCapable Info.plist
    // property to YES, otherwise you will not receive a High DPI OpenGL canvas.
    // just read:
    // https://stackoverflow.com/questions/1596945/building-osx-app-bundle
    window = SDL_CreateWindow("title", 640, 480, SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return serr.str();
    }

    int gl_major_ver       = 3;
    int gl_minor_ver       = 2;
    int gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

    const char*      platform_from_sdl = SDL_GetPlatform();
    std::string_view platform{ platform_from_sdl };
    using namespace std::string_view_literals;
    using namespace std;
    auto list = { "Windows"sv, "Mac OS X"sv };
    auto it   = std::ranges::find(list, platform);
    if (it != end(list))
    {
        gl_major_ver       = 4;
        gl_minor_ver       = (platform == "Mac OS X") ? 1 : 3;
        gl_context_profile = SDL_GL_CONTEXT_PROFILE_CORE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        gl_major_ver       = 3;
        gl_minor_ver       = 2;
        gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);
        gl_context = SDL_GL_CreateContext(window);
    }
    assert(gl_context != nullptr);

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        std::string msg("can't create opengl context: ");
        msg += SDL_GetError();
        serr << msg << endl;
        return serr.str();
    }

    bool result =
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
    assert(result);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
    assert(result);

    if (gl_major_ver <= 2 && gl_minor_ver < 1)
    {
        serr << "current context opengl version: " << gl_major_ver << '.'
             << gl_minor_ver << '\n'
             << "need opengl version at least: 2.1\n"
             << std::flush;
        return serr.str();
    }
    try
    {
        load_gl_func("glCreateShader", glCreateShader);
        load_gl_func("glShaderSource", glShaderSource);
        load_gl_func("glCompileShader", glCompileShader);
        load_gl_func("glGetShaderiv", glGetShaderiv);
        load_gl_func("glGetShaderInfoLog", glGetShaderInfoLog);
        load_gl_func("glDeleteShader", glDeleteShader);
        load_gl_func("glCreateProgram", glCreateProgram);
        load_gl_func("glAttachShader", glAttachShader);
        load_gl_func("glBindAttribLocation", glBindAttribLocation);
        load_gl_func("glLinkProgram", glLinkProgram);
        load_gl_func("glGetProgramiv", glGetProgramiv);
        load_gl_func("glGetProgramInfoLog", glGetProgramInfoLog);
        load_gl_func("glDeleteProgram", glDeleteProgram);
        load_gl_func("glUseProgram", glUseProgram);
        load_gl_func("glVertexAttribPointer", glVertexAttribPointer);
        load_gl_func("glEnableVertexAttribArray", glEnableVertexAttribArray);
        load_gl_func("glValidateProgram", glValidateProgram);
    }
    catch (std::exception& ex)
    {
        return ex.what();
    }
    // create vertex shader

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    OM_GL_CHECK()
    string_view vertex_shader_src = R"(
                                    #version 300 es
                                    in vec3 a_position;
                                    out vec4 v_position;

                                    void main()
                                    {
                                        v_position = vec4(a_position, 1.0);
                                        gl_Position = v_position;
                                    }
                                    )";
    const char* source            = vertex_shader_src.data();
    glShaderSource(vert_shader, 1, &source, nullptr);
    OM_GL_CHECK()

    glCompileShader(vert_shader);
    OM_GL_CHECK()

    GLint compiled_status = 0;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled_status);
    OM_GL_CHECK()
    if (compiled_status == 0)
    {
        GLint info_len = 0;
        glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_len);
        OM_GL_CHECK()
        std::vector<char> info_chars(static_cast<size_t>(info_len));
        glGetShaderInfoLog(vert_shader, info_len, nullptr, info_chars.data());
        OM_GL_CHECK()
        glDeleteShader(vert_shader);
        OM_GL_CHECK()

        std::string shader_type_name = "vertex";
        serr << "Error compiling " << shader_type_name << "\n"
             << vertex_shader_src << "\n"
             << info_chars.data();
        return serr.str();
    }

    // create fragment shader

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    OM_GL_CHECK()
    string_view fragment_shader_src = R"(
                      #version 300 es
                      precision mediump float;

                      in vec4 v_position;

                      out vec4 frag_color;

                      // try main_one function name on linux mesa drivers
                      void main()
                      {
                          if (v_position.z >= 0.0)
                          {
                              float light_green = 0.5 + v_position.z / 2.0;
                              frag_color = vec4(0.0, light_green, 0.0, 1.0);
                          } else
                          {
                              float color = 0.5 - (v_position.z / -2.0);
                              frag_color = vec4(color, 0.0, 0.0, 1.0);
                          }
                      }
                      )";
    source                          = fragment_shader_src.data();
    glShaderSource(fragment_shader, 1, &source, nullptr);
    OM_GL_CHECK()

    glCompileShader(fragment_shader);
    OM_GL_CHECK()

    compiled_status = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
    OM_GL_CHECK()
    if (compiled_status == 0)
    {
        GLint info_len = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
        OM_GL_CHECK()
        std::vector<char> info_chars(static_cast<size_t>(info_len));
        glGetShaderInfoLog(
            fragment_shader, info_len, nullptr, info_chars.data());
        OM_GL_CHECK()
        glDeleteShader(fragment_shader);
        OM_GL_CHECK()

        serr << "Error compiling shader(fragment)\n"
             << vertex_shader_src << "\n"
             << info_chars.data();
        return serr.str();
    }

    // now create program and attach vertex and fragment shaders

    program_id_ = glCreateProgram();
    OM_GL_CHECK()
    if (0 == program_id_)
    {
        serr << "failed to create gl program";
        return serr.str();
    }

    glAttachShader(program_id_, vert_shader);
    OM_GL_CHECK()
    glAttachShader(program_id_, fragment_shader);
    OM_GL_CHECK()

    // bind attribute location
    glBindAttribLocation(program_id_, 0, "a_position");
    OM_GL_CHECK()
    // link program after binding attribute locations
    glLinkProgram(program_id_);
    OM_GL_CHECK()
    // Check the link status
    GLint linked_status = 0;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
    OM_GL_CHECK()
    if (linked_status == 0)
    {
        GLint infoLen = 0;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        OM_GL_CHECK()
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
        OM_GL_CHECK()
        serr << "Error linking program:\n" << infoLog.data();
        glDeleteProgram(program_id_);
        OM_GL_CHECK()
        return serr.str();
    }

    // turn on rendering with just created shader program
    glUseProgram(program_id_);
    OM_GL_CHECK()

    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

    return "";
}

bool engine_impl::read_input(event& e)
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

void engine_impl::render_triangle(const triangle& t)
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), &t.v[0]);
    OM_GL_CHECK()
    glEnableVertexAttribArray(0);
    OM_GL_CHECK()
    glValidateProgram(program_id_);
    OM_GL_CHECK()
    // Check the validate status
    GLint validate_status = 0;
    glGetProgramiv(program_id_, GL_VALIDATE_STATUS, &validate_status);
    OM_GL_CHECK()
    if (validate_status == GL_FALSE)
    {
        GLint infoLen = 0;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        OM_GL_CHECK()
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
        OM_GL_CHECK()
        std::cerr << "Error linking program:\n" << infoLog.data();
        throw std::runtime_error("error");
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    OM_GL_CHECK()
}

void engine_impl::swap_buffers()
{
    SDL_GL_SwapWindow(window);

    glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
    OM_GL_CHECK()
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OM_GL_CHECK()
}

void engine_impl::uninitialize()
{
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

} // end namespace om
