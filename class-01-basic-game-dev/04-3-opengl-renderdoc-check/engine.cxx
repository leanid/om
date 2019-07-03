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

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>

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
// RENDER_DOC//////////////////////
PFNGLBINDBUFFERPROC      glBindBuffer      = nullptr;
PFNGLGENBUFFERSPROC      glGenBuffers      = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLBUFFERDATAPROC      glBufferData      = nullptr;
// RENDER_DOC//////////////////////

template <typename T>
static void load_gl_func(const char* func_name, T& result)
{
    void* gl_pointer = SDL_GL_GetProcAddress(func_name);
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
    is >> v.z;

    is >> v.r;
    is >> v.g;
    is >> v.b;

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

engine::~engine() {}

std::string engine_impl::initialize(std::string_view)
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

    window =
        SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 640, 480, ::SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return serr.str();
    }

    // RENDER_DOC//////// next code needed for RenderDoc to work
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    // RENDER_DOC////////////////////////////////////////////////
    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        std::string msg("can't create opengl context: ");
        msg += SDL_GetError();
        serr << msg << endl;
        return serr.str();
    }

    int gl_major_ver = 0;
    int result =
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
    assert(result == 0);
    int gl_minor_ver = 0;
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
    assert(result == 0);

    std::cout << "gl_context: " << gl_major_ver << '.' << gl_minor_ver
              << std::endl;

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
        load_gl_func("glBindBuffer", glBindBuffer);           // for RENDER_DOC
        load_gl_func("glGenBuffers", glGenBuffers);           // for RENDER_DOC
        load_gl_func("glGenVertexArrays", glGenVertexArrays); // for RENDER_DOC
        load_gl_func("glBindVertexArray", glBindVertexArray); // for RENDER_DOC
        load_gl_func("glBufferData", glBufferData);           // for RENDER_DOC
    }
    catch (std::exception& ex)
    {
        return ex.what();
    }

    // RENDER_DOC///////////////////////////////////////////
    GLuint vertex_buffer = 0;
    glGenBuffers(1, &vertex_buffer);
    OM_GL_CHECK();
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    OM_GL_CHECK();
    GLuint vertex_array_object = 0;
    glGenVertexArrays(1, &vertex_array_object);
    OM_GL_CHECK();
    glBindVertexArray(vertex_array_object);
    OM_GL_CHECK();
    // RENDER_DOC///////////////////////////////////////////
    // create vertex shader

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    OM_GL_CHECK();
    string_view vertex_shader_src = R"(#version 330 core
                                    layout (location = 0) in vec3 a_position;
                                    layout (location = 1) in vec3 a_color;
                                    out vec4 v_position;
                                    out vec3 v_color;
                                    void main()
                                    {
                                        v_position = vec4(a_position, 1.0);
                                        v_color = a_color;
                                        gl_Position = v_position;
                                    }
                                    )";
    const char* source            = vertex_shader_src.data();
    glShaderSource(vert_shader, 1, &source, nullptr);
    OM_GL_CHECK();

    glCompileShader(vert_shader);
    OM_GL_CHECK();

    GLint compiled_status = 0;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled_status);
    OM_GL_CHECK();
    if (compiled_status == 0)
    {
        GLint info_len = 0;
        glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_len);
        OM_GL_CHECK();
        std::vector<char> info_chars(static_cast<size_t>(info_len));
        glGetShaderInfoLog(vert_shader, info_len, nullptr, info_chars.data());
        OM_GL_CHECK();
        glDeleteShader(vert_shader);
        OM_GL_CHECK();

        std::string shader_type_name = "vertex";
        serr << "Error compiling shader(vertex)\n"
             << vertex_shader_src << "\n"
             << info_chars.data();
        return serr.str();
    }

    // create fragment shader

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    OM_GL_CHECK();
    string_view fragment_shader_src = R"(#version 330 core
                                      in vec4 v_position;
                                      in vec3 v_color;
                                      out vec4 FragColor;
                                      void main()
                                      {
                                          FragColor = vec4(v_color, 1.0);
                                          /*
                                          if (v_position.z >= 0.0)
                                          {
                                            float light_green = 0.5 + v_position.z / 2.0;
                                            FragColor = vec4(0.0, light_green, 0.0, 1.0);
                                          } else
                                          {
                                            float dark_green = 0.5 - (v_position.z / -2.0);
                                            FragColor = vec4(0.0, dark_green, 0.0, 1.0);
                                          }
                                          */
                                      }
                                      )";
    source                          = fragment_shader_src.data();
    glShaderSource(fragment_shader, 1, &source, nullptr);
    OM_GL_CHECK();

    glCompileShader(fragment_shader);
    OM_GL_CHECK();

    compiled_status = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
    OM_GL_CHECK();
    if (compiled_status == 0)
    {
        GLint info_len = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
        OM_GL_CHECK();
        std::vector<char> info_chars(static_cast<size_t>(info_len));
        glGetShaderInfoLog(fragment_shader, info_len, nullptr,
                           info_chars.data());
        OM_GL_CHECK();
        glDeleteShader(fragment_shader);
        OM_GL_CHECK();

        serr << "Error compiling shader(fragment)\n"
             << vertex_shader_src << "\n"
             << info_chars.data();
        return serr.str();
    }

    // now create program and attach vertex and fragment shaders

    program_id_ = glCreateProgram();
    OM_GL_CHECK();
    if (0 == program_id_)
    {
        serr << "failed to create gl program";
        return serr.str();
    }

    glAttachShader(program_id_, vert_shader);
    OM_GL_CHECK();
    glAttachShader(program_id_, fragment_shader);
    OM_GL_CHECK();

    // bind attribute location
    glBindAttribLocation(program_id_, 0, "a_position");
    OM_GL_CHECK();
    glBindAttribLocation(program_id_, 1, "a_color");
    OM_GL_CHECK();
    // link program after binding attribute locations
    glLinkProgram(program_id_);
    OM_GL_CHECK();
    // Check the link status
    GLint linked_status = 0;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
    OM_GL_CHECK();
    if (linked_status == 0)
    {
        GLint infoLen = 0;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        OM_GL_CHECK();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
        OM_GL_CHECK();
        serr << "Error linking program:\n" << infoLog.data();
        glDeleteProgram(program_id_);
        OM_GL_CHECK();
        return serr.str();
    }

    // turn on rendering with just created shader program
    glUseProgram(program_id_);
    OM_GL_CHECK();

    glEnable(GL_DEPTH_TEST);
    OM_GL_CHECK();

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

void engine_impl::render_triangle(const triangle& t)
{
    glBufferData(GL_ARRAY_BUFFER, sizeof(t), &t, GL_STATIC_DRAW);
    OM_GL_CHECK();
    glEnableVertexAttribArray(0);

    GLintptr position_attr_offset = 0;

    OM_GL_CHECK();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          reinterpret_cast<void*>(position_attr_offset));
    OM_GL_CHECK();
    glEnableVertexAttribArray(1);
    OM_GL_CHECK();

    GLintptr color_attr_offset = sizeof(float) * 3;

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          reinterpret_cast<void*>(color_attr_offset));
    OM_GL_CHECK();
    glValidateProgram(program_id_);
    OM_GL_CHECK();
    // Check the validate status
    GLint validate_status = 0;
    glGetProgramiv(program_id_, GL_VALIDATE_STATUS, &validate_status);
    OM_GL_CHECK();
    if (validate_status == GL_FALSE)
    {
        GLint infoLen = 0;
        glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        OM_GL_CHECK();
        std::vector<char> infoLog(static_cast<size_t>(infoLen));
        glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
        OM_GL_CHECK();
        std::cerr << "Error linking program:\n" << infoLog.data();
        throw std::runtime_error("error");
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    OM_GL_CHECK();
}

void engine_impl::swap_buffers()
{
    SDL_GL_SwapWindow(window);

    glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
    OM_GL_CHECK();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OM_GL_CHECK();
}

void engine_impl::uninitialize()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

} // end namespace om
