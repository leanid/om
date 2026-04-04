#include "engine.hxx"

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_stdinc.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <SDL3/SDL_video.h>

#include "picopng.hxx"

#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"

struct ImDrawData;
void imgui_ensure_device_objects();
void imgui_to_engine_render(ImDrawData* draw_data);
void imgui_invalidate_device_objects();

// we have to load all extension GL function pointers
// dynamically from OpenGL library
// so first declare function pointers for all we need
static PFNGLCREATESHADERPROC             glCreateShader             = nullptr;
static PFNGLSHADERSOURCEPROC             glShaderSource             = nullptr;
static PFNGLCOMPILESHADERPROC            glCompileShader            = nullptr;
static PFNGLGETSHADERIVPROC              glGetShaderiv              = nullptr;
static PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog         = nullptr;
static PFNGLDELETESHADERPROC             glDeleteShader             = nullptr;
static PFNGLCREATEPROGRAMPROC            glCreateProgram            = nullptr;
static PFNGLATTACHSHADERPROC             glAttachShader             = nullptr;
static PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation       = nullptr;
static PFNGLLINKPROGRAMPROC              glLinkProgram              = nullptr;
static PFNGLGETPROGRAMIVPROC             glGetProgramiv             = nullptr;
static PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog        = nullptr;
static PFNGLDELETEPROGRAMPROC            glDeleteProgram            = nullptr;
static PFNGLUSEPROGRAMPROC               glUseProgram               = nullptr;
static PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = nullptr;
static PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = nullptr;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
static PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation       = nullptr;
static PFNGLUNIFORM1IPROC                glUniform1i                = nullptr;
static PFNGLACTIVETEXTUREPROC            glActiveTexture_           = nullptr;
static PFNGLUNIFORM4FVPROC               glUniform4fv               = nullptr;
static PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv         = nullptr;
static PFNGLGENBUFFERSPROC               glGenBuffers               = nullptr;
static PFNGLBINDBUFFERPROC               glBindBuffer               = nullptr;
static PFNGLBUFFERDATAPROC               glBufferData               = nullptr;
static PFNGLBUFFERSUBDATAPROC            glBufferSubData            = nullptr;
static PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv         = nullptr;
static PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate    = nullptr;
static PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate        = nullptr;
static PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation        = nullptr;
static PFNGLDELETEBUFFERSPROC            glDeleteBuffers            = nullptr;
static PFNGLDETACHSHADERPROC             glDetachShader             = nullptr;

template <typename T> static void load_gl_func(const char* func_name, T& result)
{
    SDL_FunctionPointer gl_pointer = SDL_GL_GetProcAddress(func_name);
    if (nullptr == gl_pointer)
    {
        throw std::runtime_error(std::string("can't load GL function: ") +
                                 func_name);
    }
    result = reinterpret_cast<T>(gl_pointer);
}

#define OM_GL_CHECK()                                                          \
    {                                                                          \
        const unsigned int err = glGetError();                                 \
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
            assert(false);                                                     \
        }                                                                      \
    }

namespace om
{

vec2::vec2()

    = default;
vec2::vec2(float x_, float y_)
    : x(x_)
    , y(y_)
{
}

vec2 operator+(const vec2& l, const vec2& r)
{
    vec2 result;
    result.x = l.x + r.x;
    result.y = l.y + r.y;
    return result;
}

mat2x3::mat2x3()
    : col0(1.0f, 0.f)
    , col1(0.f, 1.f)
    , delta(0.f, 0.f)
{
}

mat2x3 mat2x3::identity()
{
    return mat2x3::scale(1.f);
}

mat2x3 mat2x3::scale(float scale)
{
    mat2x3 result;
    result.col0.x = scale;
    result.col1.y = scale;
    return result;
}

mat2x3 mat2x3::scale(float sx, float sy)
{
    mat2x3 r;
    r.col0.x = sx;
    r.col1.y = sy;
    return r;
}

mat2x3 mat2x3::rotation(float thetha)
{
    mat2x3 result;

    result.col0.x = std::cos(thetha);
    result.col0.y = std::sin(thetha);

    result.col1.x = -std::sin(thetha);
    result.col1.y = std::cos(thetha);

    return result;
}

mat2x3 mat2x3::move(const vec2& delta)
{
    mat2x3 r = mat2x3::identity();
    r.delta  = delta;
    return r;
}

vec2 operator*(const vec2& v, const mat2x3& m)
{
    vec2 result;
    result.x = v.x * m.col0.x + v.y * m.col0.y + m.delta.x;
    result.y = v.x * m.col1.x + v.y * m.col1.y + m.delta.y;
    return result;
}

mat2x3 operator*(const mat2x3& m1, const mat2x3& m2)
{
    mat2x3 r;

    r.col0.x = m1.col0.x * m2.col0.x + m1.col1.x * m2.col0.y;
    r.col1.x = m1.col0.x * m2.col1.x + m1.col1.x * m2.col1.y;
    r.col0.y = m1.col0.y * m2.col0.x + m1.col1.y * m2.col0.y;
    r.col1.y = m1.col0.y * m2.col1.x + m1.col1.y * m2.col1.y;

    r.delta.x = m1.delta.x * m2.col0.x + m1.delta.y * m2.col0.y + m2.delta.x;
    r.delta.y = m1.delta.x * m2.col1.x + m1.delta.y * m2.col1.y + m2.delta.y;

    return r;
}

texture::~texture() = default;

vertex_buffer::~vertex_buffer() = default;
index_buffer::~index_buffer()   = default;

class vertex_buffer_impl final : public vertex_buffer
{
public:
    vertex_buffer_impl(const tri2* tri, std::size_t n)
        : count(static_cast<std::uint32_t>(n * 3))
    {
        glGenBuffers(1, &gl_handle);
        OM_GL_CHECK()

        bind();

        auto size_in_bytes = static_cast<GLsizeiptr>(n * 3 * sizeof(v2));

        glBufferData(
            GL_ARRAY_BUFFER, size_in_bytes, &tri->v[0], GL_STATIC_DRAW);
        OM_GL_CHECK()
    }
    vertex_buffer_impl(const v2* vert, std::size_t n)
        : count(static_cast<std::uint32_t>(n))
    {
        glGenBuffers(1, &gl_handle);
        OM_GL_CHECK()

        bind();

        auto size_in_bytes = static_cast<GLsizeiptr>(n * sizeof(v2));

        glBufferData(GL_ARRAY_BUFFER, size_in_bytes, vert, GL_STATIC_DRAW);
        OM_GL_CHECK()
    }
    ~vertex_buffer_impl() final;

    void bind() const override
    {
        glBindBuffer(GL_ARRAY_BUFFER, gl_handle);
        OM_GL_CHECK()
    }

    [[nodiscard]] std::uint32_t size() const override { return count; }

private:
    std::uint32_t gl_handle{ 0 };
    std::uint32_t count{ 0 };
};

class index_buffer_impl final : public index_buffer
{
public:
    index_buffer_impl(const std::uint16_t* i, size_t n);
    ~index_buffer_impl() override;
    void bind() const override
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_handle);
        OM_GL_CHECK();
    }

    [[nodiscard]] std::uint32_t size() const override { return count; }

private:
    std::uint32_t gl_handle{ 0 };
    std::uint32_t count{ 0 };
};

#pragma pack(push, 4)
class texture_gl_es20 final : public texture
{
public:
    explicit texture_gl_es20(std::string_view path);
    texture_gl_es20(const void*  pixels,
                    const size_t width,
                    const size_t height);
    ~texture_gl_es20() override;

    void bind() const override
    {
        glBindTexture(GL_TEXTURE_2D, tex_handl);
        OM_GL_CHECK();
    }

    [[nodiscard]] std::uint32_t get_width() const final { return width; }
    [[nodiscard]] std::uint32_t get_height() const final { return height; }

private:
    void gen_texture_from_pixels(const void*  pixels,
                                 const size_t width,
                                 const size_t height);

    std::string   file_path;
    GLuint        tex_handl = 0;
    std::uint32_t width     = 0;
    std::uint32_t height    = 0;
};
#pragma pack(pop)

class shader_gl_es20
{
public:
    shader_gl_es20(
        std::string_view vertex_src, // NOLINT
        std::string_view fragment_src,
        const std::vector<std::tuple<GLuint, const GLchar*>>& attributes)
    {
        vert_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
        frag_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
        if (vert_shader == 0 || frag_shader == 0)
        {
            throw std::runtime_error("can't compile shader");
        }
        program_id = link_shader_program(attributes);
        if (program_id == 0)
        {
            throw std::runtime_error("can't link shader");
        }
    }

    void use() const
    {
        glUseProgram(program_id);
        OM_GL_CHECK();
    }

    void set_uniform(std::string_view uniform_name, texture_gl_es20* texture)
    {
        assert(texture != nullptr);
        const int location =
            glGetUniformLocation(program_id, uniform_name.data()); // NOLINT
        OM_GL_CHECK();
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        unsigned int texture_unit = 0;
        glActiveTexture_(GL_TEXTURE0 + texture_unit);
        OM_GL_CHECK();

        texture->bind();

        // http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
        glUniform1i(location, static_cast<int>(0 + texture_unit));
        OM_GL_CHECK();
    }

    void set_uniform(std::string_view uniform_name, const color& c)
    {
        const int location =
            glGetUniformLocation(program_id, uniform_name.data()); // NOLINT
        OM_GL_CHECK();
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        // NOLINTNEXTLINE
        float values[4] = { c.get_r(), c.get_g(), c.get_b(), c.get_a() };
        glUniform4fv(location, 1, &values[0]);
        OM_GL_CHECK();
    }

    void set_uniform(std::string_view uniform_name, const mat2x3& m)
    {
        const int location =
            glGetUniformLocation(program_id, uniform_name.data()); // NOLINT
        OM_GL_CHECK();
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        // OpenGL wants matrix in column major order
        // clang-format off
        // NOLINTNEXTLINE
        float values[9] = { m.col0.x,  m.col0.y,  0.f,
                            m.col1.x,  m.col1.y,  0.f,
                            m.delta.x, m.delta.y, 1.f };
        // clang-format on
        glUniformMatrix3fv(location, 1, GL_FALSE, &values[0]);
        OM_GL_CHECK();
    }

    [[nodiscard]] GLuint get_program_id() const { return program_id; }

private:
    GLuint compile_shader(GLenum shader_type, std::string_view src)
    {
        GLuint shader_id = glCreateShader(shader_type);
        OM_GL_CHECK();
        std::string_view vertex_shader_src = src;
        const char*      source            = vertex_shader_src.data();
        glShaderSource(shader_id, 1, &source, nullptr);
        OM_GL_CHECK();

        glCompileShader(shader_id);
        OM_GL_CHECK();

        GLint compiled_status = 0;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK();
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK();
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(shader_id, info_len, nullptr, info_chars.data());
            OM_GL_CHECK();
            glDeleteShader(shader_id);
            OM_GL_CHECK();

            std::string shader_type_name =
                shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment";
            std::cerr << "Error compiling shader(" << shader_type_name << ")\n"
                      << vertex_shader_src << "\n"
                      << info_chars.data();
            return 0;
        }
        return shader_id;
    }
    GLuint link_shader_program(
        const std::vector<std::tuple<GLuint, const GLchar*>>& attributes)
    {
        GLuint program_id_ = glCreateProgram();
        OM_GL_CHECK();
        if (0 == program_id_)
        {
            std::cerr << "failed to create gl program";
            throw std::runtime_error("can't link shader");
        }

        glAttachShader(program_id_, vert_shader);
        OM_GL_CHECK();
        glAttachShader(program_id_, frag_shader);
        OM_GL_CHECK();

        // bind attribute location
        for (const auto& attr : attributes)
        {
            GLuint        loc  = std::get<0>(attr);
            const GLchar* name = std::get<1>(attr);
            glBindAttribLocation(program_id_, loc, name);
            OM_GL_CHECK();
        }

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
            std::cerr << "Error linking program:\n" << infoLog.data();
            glDeleteProgram(program_id_);
            OM_GL_CHECK();
            return 0;
        }
        return program_id_;
    }

    GLuint vert_shader = 0;
    GLuint frag_shader = 0;
    GLuint program_id  = 0;
};

static std::array<std::string_view, 17> event_names = {
    /// input events
    { "left_pressed",
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

tri0::tri0()
    : v{ v0(), v0(), v0() }
{
}

tri1::tri1()
    : v{ v1(), v1(), v1() }
{
}

tri2::tri2()
    : v{ v2(), v2(), v2() }
{
}

std::istream& operator>>(std::istream& is, mat2x3& m)
{
    is >> m.col0.x;
    is >> m.col1.x;
    is >> m.col0.y;
    is >> m.col1.y;
    return is;
}

std::istream& operator>>(std::istream& is, vec2& v)
{
    is >> v.x;
    is >> v.y;
    return is;
}

std::istream& operator>>(std::istream& is, color& c)
{
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    float a = 0.f;
    is >> r;
    is >> g;
    is >> b;
    is >> a;
    c = color(r, g, b, a);
    return is;
}

std::istream& operator>>(std::istream& is, v0& v)
{
    is >> v.pos.x;
    is >> v.pos.y;

    return is;
}

std::istream& operator>>(std::istream& is, v1& v)
{
    is >> v.pos.x;
    is >> v.pos.y;
    is >> v.c;
    return is;
}

std::istream& operator>>(std::istream& is, v2& v)
{
    is >> v.pos.x;
    is >> v.pos.y;
    is >> v.uv;
    is >> v.c;
    return is;
}

std::istream& operator>>(std::istream& is, tri0& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

std::istream& operator>>(std::istream& is, tri1& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

std::istream& operator>>(std::istream& is, tri2& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

struct bind
{
    bind(std::string_view s,
         SDL_Keycode      k,
         event            pressed, // NOLINT
         event            released,
         keys             om_k)
        : name(s)
        , key(k)
        , event_pressed(pressed)
        , event_released(released)
        , om_key(om_k)
    {
    }

    std::string_view name;
    SDL_Keycode      key;

    event event_pressed;
    event event_released;

    om::keys om_key;
};

const std::array<bind, 8> keys{
    { bind{ "up", SDLK_W, event::up_pressed, event::up_released, keys::up },
      bind{ "left",
            SDLK_A,
            event::left_pressed,
            event::left_released,
            keys::left },
      bind{ "down",
            SDLK_S,
            event::down_pressed,
            event::down_released,
            keys::down },
      bind{ "right",
            SDLK_D,
            event::right_pressed,
            event::right_released,
            keys::right },
      bind{ "button1",
            SDLK_LCTRL,
            event::button1_pressed,
            event::button1_released,
            keys::button1 },
      bind{ "button2",
            SDLK_SPACE,
            event::button2_pressed,
            event::button2_released,
            keys::button2 },
      bind{ "select",
            SDLK_ESCAPE,
            event::select_pressed,
            event::select_released,
            keys::select },
      bind{ "start",
            SDLK_RETURN,
            event::start_pressed,
            event::start_released,
            keys::start } }
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

#pragma pack(push, 1)
class engine_impl final : public engine
{
public:
    /// create main window
    /// on success return empty string
    std::string initialize(std::string_view /*config*/) final;
    /// return seconds from initialization
    float get_time_from_init() final
    {
        std::uint32_t ms_from_library_initialization = SDL_GetTicks();
        float seconds = ms_from_library_initialization * 0.001f; // NOLINT
        return seconds;
    }
    /// pool event from input queue
    /// return true if more events in queue
    bool read_event(event& e) final
    {
        using namespace std;
        // collect all events from SDL
        SDL_Event sdl_event;
        if (SDL_PollEvent(&sdl_event))
        {
            ImGui_ImplSDL3_ProcessEvent(&sdl_event);

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

    bool is_key_down(const enum keys key) final
    {
        const auto it = std::ranges::find_if(keys,

                                             [&](const bind& b)
                                             { return b.om_key == key; });

        if (it != end(keys))
        {
            const bool* state = SDL_GetKeyboardState(nullptr);
            SDL_Keymod  mod{};
            int         sdl_scan_code = SDL_GetScancodeFromKey(it->key, &mod);
            return state[sdl_scan_code];
        }
        return false;
    }

    texture* create_texture(std::string_view path) final
    {
        return new texture_gl_es20(path);
    }
    texture* create_texture_rgba32(const void*  pixels,
                                   const size_t width,
                                   const size_t height) override
    {
        return new texture_gl_es20(pixels, width, height);
    }
    void destroy_texture(texture* t) final { delete t; }

    vertex_buffer* create_vertex_buffer(const tri2* triangles,
                                        std::size_t n) override
    {
        assert(triangles != nullptr);
        return new vertex_buffer_impl(triangles, n);
    }
    vertex_buffer* create_vertex_buffer(const v2*   vert,
                                        std::size_t count) override
    {
        assert(vert != nullptr);
        return new vertex_buffer_impl(vert, count);
    }
    void destroy_vertex_buffer(vertex_buffer* buffer) override
    {
        delete buffer;
    }

    index_buffer* create_index_buffer(const std::uint16_t* indexes,
                                      std::size_t          count) override
    {
        return new index_buffer_impl(indexes, count);
    }
    void destroy_index_buffer(index_buffer* buffer) override { delete buffer; }

    void render(const tri0& t, const color& c) final
    {
        shader00->use();
        shader00->set_uniform("u_color", c);
        // vertex coordinates
        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, sizeof(v0), &t.v[0].pos.x);
        OM_GL_CHECK();
        glEnableVertexAttribArray(0);
        OM_GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        OM_GL_CHECK();
    }
    void render(const tri1& t) final
    {
        shader01->use();
        // positions
        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]), &t.v[0].pos);
        OM_GL_CHECK();
        glEnableVertexAttribArray(0);
        OM_GL_CHECK();
        // colors
        glVertexAttribPointer(
            1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]), &t.v[0].c);
        OM_GL_CHECK();
        glEnableVertexAttribArray(1);
        OM_GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        OM_GL_CHECK();

        glDisableVertexAttribArray(1);
        OM_GL_CHECK();
    }
    void render(const tri2& t, texture* tex) final
    {
        shader02->use();
        auto* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader02->set_uniform("s_texture", texture);
        // positions
        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]), &t.v[0].pos);
        OM_GL_CHECK();
        glEnableVertexAttribArray(0);
        OM_GL_CHECK();
        // colors
        glVertexAttribPointer(
            1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]), &t.v[0].c);
        OM_GL_CHECK();
        glEnableVertexAttribArray(1);
        OM_GL_CHECK();

        // texture coordinates
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]), &t.v[0].uv);
        OM_GL_CHECK();
        glEnableVertexAttribArray(2);
        OM_GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        OM_GL_CHECK();

        glDisableVertexAttribArray(1);
        OM_GL_CHECK();
        glDisableVertexAttribArray(2);
        OM_GL_CHECK();
    }
    void render(const tri2& t, texture* tex, const mat2x3& m) final
    {
        shader03->use();
        auto* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader03->set_uniform("s_texture", texture);
        shader03->set_uniform("u_matrix", m);
        // positions
        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]), &t.v[0].pos);
        OM_GL_CHECK();
        glEnableVertexAttribArray(0);
        OM_GL_CHECK();
        // colors
        glVertexAttribPointer(
            1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]), &t.v[0].c);
        OM_GL_CHECK();
        glEnableVertexAttribArray(1);
        OM_GL_CHECK();

        // texture coordinates
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]), &t.v[0].uv);
        OM_GL_CHECK();
        glEnableVertexAttribArray(2);
        OM_GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        OM_GL_CHECK();

        glDisableVertexAttribArray(1);
        OM_GL_CHECK();
        glDisableVertexAttribArray(2);
        OM_GL_CHECK();
    }
    void render(const vertex_buffer& buff, texture* tex, const mat2x3& m) final
    {
        shader03->use();
        auto* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader03->set_uniform("s_texture", texture);
        shader03->set_uniform("u_matrix", m);

        buff.bind();

        // positions
        glEnableVertexAttribArray(0);
        OM_GL_CHECK();
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), nullptr);
        OM_GL_CHECK();
        // colors
        glVertexAttribPointer(
            1,
            4,
            GL_UNSIGNED_BYTE,
            GL_TRUE,
            sizeof(v2),
            reinterpret_cast<void*>(sizeof(v2::pos) + sizeof(v2::uv)));
        OM_GL_CHECK();
        glEnableVertexAttribArray(1);
        OM_GL_CHECK();

        // texture coordinates
        glVertexAttribPointer(2,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(v2),
                              reinterpret_cast<void*>(sizeof(v2::pos)));
        OM_GL_CHECK();
        glEnableVertexAttribArray(2);
        OM_GL_CHECK();

        auto num_of_vertexes = static_cast<GLsizei>(buff.size());
        glDrawArrays(GL_TRIANGLES, 0, num_of_vertexes);
        OM_GL_CHECK();

        glDisableVertexAttribArray(1);
        OM_GL_CHECK();
        glDisableVertexAttribArray(2);
        OM_GL_CHECK();
    }

    void render(const vertex_buffer* buff,
                const index_buffer*  indexes,
                const texture*       tex,
                const std::uint16_t* start_vertex_index,
                size_t               num_vertexes) override
    {
        tex->bind();

        buff->bind();

        indexes->bind();

        glEnableVertexAttribArray(0); // g_AttribLocationPosition
        OM_GL_CHECK()
        glEnableVertexAttribArray(1); // g_AttribLocationUV
        OM_GL_CHECK()
        glEnableVertexAttribArray(2); // g_AttribLocationColor
        OM_GL_CHECK()

        glVertexAttribPointer(
            0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), reinterpret_cast<void*>(0));
        OM_GL_CHECK()
        glVertexAttribPointer(1,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(v2),
                              reinterpret_cast<void*>(2 * sizeof(float)));
        OM_GL_CHECK()
        glVertexAttribPointer(2,
                              4,
                              GL_UNSIGNED_BYTE,
                              GL_TRUE,
                              sizeof(v2),
                              reinterpret_cast<void*>(4 * sizeof(float)));
        OM_GL_CHECK()

        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(num_vertexes),
                       GL_UNSIGNED_SHORT,
                       start_vertex_index);

        OM_GL_CHECK()
    }

    void swap_buffers() final
    {
        imgui_ensure_device_objects();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn
        // more about
        // Dear ImGui!).

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        if (ImGui::Button("Copy move tank to right"))
        {
            std::cout << "try move tank" << std::endl;
        }

        ImGui::Render();
        imgui_to_engine_render(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);

        glClear(GL_COLOR_BUFFER_BIT);
        OM_GL_CHECK()
    }
    void uninitialize() final
    {
        imgui_invalidate_device_objects();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

private:
    SDL_Window*   window     = nullptr;
    SDL_GLContext gl_context = nullptr;

    shader_gl_es20* shader00       = nullptr;
    shader_gl_es20* shader01       = nullptr;
    shader_gl_es20* shader02       = nullptr;
    shader_gl_es20* shader03       = nullptr;
    uint32_t        gl_default_vbo = 0;
    bool            show_demo_window{ true };
};
#pragma pack(pop)

static bool    already_exist  = false;
static engine* g_imgui_engine = nullptr;

engine* create_engine()
{
    if (already_exist)
    {
        throw std::runtime_error("engine already exist");
    }
    engine* result = new engine_impl();
    g_imgui_engine = result;
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
    g_imgui_engine = nullptr;
    already_exist  = false;
}

color::color(std::uint32_t rgba_)
    : rgba(rgba_)
{
}
// NOLINTNEXTLINE
color::color(float r, float g, float b, float a)
{
    assert(r <= 1 && r >= 0);
    assert(g <= 1 && g >= 0);
    assert(b <= 1 && b >= 0);
    assert(a <= 1 && a >= 0);

    auto r_ = static_cast<std::uint32_t>(r * 255);
    auto g_ = static_cast<std::uint32_t>(g * 255);
    auto b_ = static_cast<std::uint32_t>(b * 255);
    auto a_ = static_cast<std::uint32_t>(a * 255);

    rgba = a_ << 24 | b_ << 16 | g_ << 8 | r_;
}

float color::get_r() const
{
    std::uint32_t r_ = (rgba & 0x000000FF) >> 0;
    return static_cast<float>(r_) / 255.f;
}
float color::get_g() const
{
    std::uint32_t g_ = (rgba & 0x0000FF00) >> 8;
    return static_cast<float>(g_) / 255.f;
}
float color::get_b() const
{
    std::uint32_t b_ = (rgba & 0x00FF0000) >> 16;
    return static_cast<float>(b_) / 255.f;
}
float color::get_a() const
{
    std::uint32_t a_ = (rgba & 0xFF000000) >> 24;
    return static_cast<float>(a_) / 255.f;
}

void color::set_r(const float r)
{
    auto r_ = static_cast<std::uint32_t>(r * 255);
    rgba &= 0xFFFFFF00;
    rgba |= (r_ << 0);
}
void color::set_g(const float g)
{
    auto g_ = static_cast<std::uint32_t>(g * 255);
    rgba &= 0xFFFF00FF;
    rgba |= (g_ << 8);
}
void color::set_b(const float b)
{
    auto b_ = static_cast<std::uint32_t>(b * 255);
    rgba &= 0xFF00FFFF;
    rgba |= (b_ << 16);
}
void color::set_a(const float a)
{
    auto a_ = static_cast<std::uint32_t>(a * 255);
    rgba &= 0x00FFFFFF;
    rgba |= a_ << 24;
}

engine::~engine() = default;

texture_gl_es20::texture_gl_es20(std::string_view path)
    : file_path(path)
{
    std::vector<unsigned char> png_file_in_memory;
    std::ifstream ifs(path.data(), std::ios_base::binary); // NOLINT
    if (!ifs)
    {
        throw std::runtime_error("can't load texture");
    }
    ifs.seekg(0, std::ios_base::end);
    std::streamoff pos_in_file = ifs.tellg();
    png_file_in_memory.resize(static_cast<size_t>(pos_in_file));
    ifs.seekg(0, std::ios_base::beg);
    if (!ifs)
    {
        throw std::runtime_error("can't load texture");
    }

    ifs.read(reinterpret_cast<char*>(png_file_in_memory.data()), pos_in_file);
    if (!ifs.good())
    {
        throw std::runtime_error("can't load texture");
    }

    std::vector<unsigned char> image;
    unsigned long              w     = 0;
    unsigned long              h     = 0;
    int                        error = decodePNG(
        image, w, h, &png_file_in_memory[0], png_file_in_memory.size(), false);

    // if there's an error, display it
    if (error != 0)
    {
        std::cerr << "error: " << error << std::endl;
        throw std::runtime_error("can't load texture");
    }

    gen_texture_from_pixels(image.data(), w, h);
}

void texture_gl_es20::gen_texture_from_pixels(const void*  pixels,
                                              const size_t w, // NOLINT
                                              const size_t h)
{
    glGenTextures(1, &tex_handl);
    OM_GL_CHECK();
    glBindTexture(GL_TEXTURE_2D, tex_handl);
    OM_GL_CHECK();

    GLint mipmap_level = 0;
    GLint border       = 0;
    auto  width_       = static_cast<GLsizei>(w);
    auto  height_      = static_cast<GLsizei>(h);
    glTexImage2D(GL_TEXTURE_2D,
                 mipmap_level,
                 GL_RGBA,
                 width_,
                 height_,
                 border,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels);
    OM_GL_CHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OM_GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OM_GL_CHECK();
}

texture_gl_es20::texture_gl_es20(const void*  pixels,
                                 const size_t w,
                                 const size_t h)
{
    gen_texture_from_pixels(pixels, w, h);
    if (file_path.empty())
    {
        file_path = "::memory::";
    }
}

texture_gl_es20::~texture_gl_es20()
{
    glDeleteTextures(1, &tex_handl);
    OM_GL_CHECK();
}

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

    window = SDL_CreateWindow("title", 800, 600, SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        serr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return serr.str();
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            std::string msg("can't create opengl context: ");
            msg += SDL_GetError();
            serr << msg << endl;
            return serr.str();
        }
    }

    int  gl_major_ver = 0;
    bool result =
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
    assert(result);
    int gl_minor_ver = 0;
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
    assert(result);

    if (gl_major_ver < 2)
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
        load_gl_func("glDisableVertexAttribArray", glDisableVertexAttribArray);
        load_gl_func("glGetUniformLocation", glGetUniformLocation);
        load_gl_func("glUniform1i", glUniform1i);
        load_gl_func("glActiveTexture", glActiveTexture_);
        load_gl_func("glUniform4fv", glUniform4fv);
        load_gl_func("glUniformMatrix3fv", glUniformMatrix3fv);
        load_gl_func("glGenBuffers", glGenBuffers);
        load_gl_func("glBindBuffer", glBindBuffer);
        load_gl_func("glBufferData", glBufferData);
        load_gl_func("glBufferSubData", glBufferSubData);
        load_gl_func("glUniformMatrix4fv", glUniformMatrix4fv);
        load_gl_func("glBlendEquationSeparate", glBlendEquationSeparate);
        load_gl_func("glBlendFuncSeparate", glBlendFuncSeparate);
        load_gl_func("glGetAttribLocation", glGetAttribLocation);
        load_gl_func("glDeleteBuffers", glDeleteBuffers);
        load_gl_func("glDetachShader", glDetachShader);
    }
    catch (std::exception& ex)
    {
        return ex.what();
    }

    glGenBuffers(1, &gl_default_vbo);
    OM_GL_CHECK();
    glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
    OM_GL_CHECK();
    uint32_t data_size_in_bytes = 0;
    glBufferData(GL_ARRAY_BUFFER, data_size_in_bytes, nullptr, GL_STATIC_DRAW);
    OM_GL_CHECK();
    glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, nullptr);
    OM_GL_CHECK();

    shader00 = new shader_gl_es20(R"(
                                  attribute vec2 a_position;
                                  void main()
                                  {
                                      gl_Position = vec4(a_position, 0.0, 1.0);
                                  }
                                  )",
                                  R"(
                                  #ifdef GL_ES
                                  precision mediump float;
                                  #endif
                                  uniform vec4 u_color;
                                  void main()
                                  {
                                      gl_FragColor = u_color;
                                  }
                                  )",
                                  { { 0, "a_position" } });

    shader00->use();
    shader00->set_uniform("u_color", color(1.f, 0.f, 0.f, 1.f));

    shader01 = new shader_gl_es20(
        R"(
                attribute vec2 a_position;
                attribute vec4 a_color;
                varying vec4 v_color;
                void main()
                {
                v_color = a_color;
                gl_Position = vec4(a_position, 0.0, 1.0);
                }
                )",
        R"(
                #ifdef GL_ES
                precision mediump float;
                #endif
                varying vec4 v_color;
                void main()
                {
                gl_FragColor = v_color;
                }
                )",
        { { 0, "a_position" }, { 1, "a_color" } });

    shader01->use();

    shader02 = new shader_gl_es20(
        R"(
                attribute vec2 a_position;
                attribute vec2 a_tex_coord;
                attribute vec4 a_color;
                varying vec4 v_color;
                varying vec2 v_tex_coord;
                void main()
                {
                v_tex_coord = a_tex_coord;
                v_color = a_color;
                gl_Position = vec4(a_position, 0.0, 1.0);
                }
                )",
        R"(
                #ifdef GL_ES
                precision mediump float;
                #endif
                varying vec2 v_tex_coord;
                varying vec4 v_color;
                uniform sampler2D s_texture;
                void main()
                {
                gl_FragColor = texture2D(s_texture, v_tex_coord) * v_color;
                }
                )",
        { { 0, "a_position" }, { 1, "a_color" }, { 2, "a_tex_coord" } });

    // turn on rendering with just created shader program
    shader02->use();

    shader03 = new shader_gl_es20(
        R"(
                uniform mat3 u_matrix;
                attribute vec2 a_position;
                attribute vec2 a_tex_coord;
                attribute vec4 a_color;
                varying vec4 v_color;
                varying vec2 v_tex_coord;
                void main()
                {
                v_tex_coord = a_tex_coord;
                v_color = a_color;
                vec3 pos = vec3(a_position, 1.0) * u_matrix;
                gl_Position = vec4(pos, 1.0);
                }
                )",
        R"(
                #ifdef GL_ES
                precision mediump float;
                #endif
                varying vec2 v_tex_coord;
                varying vec4 v_color;
                uniform sampler2D s_texture;
                void main()
                {
                gl_FragColor = texture2D(s_texture, v_tex_coord) * v_color;
                }
                )",
        { { 0, "a_position" }, { 1, "a_color" }, { 2, "a_tex_coord" } });
    shader03->use();

    glEnable(GL_BLEND);
    OM_GL_CHECK()
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    OM_GL_CHECK()

    glClearColor(0.f, 0.0, 0.f, 0.0f);
    OM_GL_CHECK()

    glViewport(0, 0, 800, 600);
    OM_GL_CHECK()

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().Fonts->AddFontDefault();
    if (!ImGui_ImplSDL3_InitForOpenGL(window, gl_context))
    {
        return "error: failed to init ImGui (SDL3)";
    }

    return "";
}

index_buffer_impl::index_buffer_impl(const uint16_t* i, size_t n)
    : count(static_cast<std::uint32_t>(n))
{
    glGenBuffers(1, &gl_handle);
    OM_GL_CHECK()

    bind();

    auto size_in_bytes = static_cast<GLsizeiptr>(n * sizeof(std::uint16_t));

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_in_bytes, i, GL_STATIC_DRAW);
    OM_GL_CHECK()
}

index_buffer_impl::~index_buffer_impl()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    OM_GL_CHECK()
    glDeleteBuffers(1, &gl_handle);
    OM_GL_CHECK()
}

vertex_buffer_impl::~vertex_buffer_impl()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    OM_GL_CHECK()

    glDeleteBuffers(1, &gl_handle);
    OM_GL_CHECK()
}

} // end namespace om
// Dear ImGui: custom rendering through om::engine (textured indexed triangles),
// SDL3 input via imgui_impl_sdl3 only.

static float               g_imgui_time          = 0.0F;
static bool                g_imgui_mouse_down[3] = { false, false, false };
static float               g_imgui_mouse_wheel   = 0.0F;
static om::shader_gl_es20* g_imgui_shader        = nullptr;
static om::texture*        g_imgui_font_texture  = nullptr;
static void                imgui_destroy_font_texture()
{
    if (g_imgui_font_texture != nullptr && om::g_imgui_engine != nullptr)
    {
        om::g_imgui_engine->destroy_texture(g_imgui_font_texture);
        g_imgui_font_texture = nullptr;
    }
}

void imgui_invalidate_device_objects()
{
    imgui_destroy_font_texture();
    delete g_imgui_shader;
    g_imgui_shader = nullptr;
}

static bool imgui_create_font_texture()
{
    ImGuiIO&       io     = ImGui::GetIO();
    unsigned char* pixels = nullptr;
    int            w      = 0;
    int            h      = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &w, &h);
    g_imgui_font_texture = om::g_imgui_engine->create_texture_rgba32(
        pixels, static_cast<size_t>(w), static_cast<size_t>(h));
    io.Fonts->SetTexID(static_cast<ImTextureID>(
        reinterpret_cast<uintptr_t>(g_imgui_font_texture)));
    return true;
}

static bool imgui_create_device_objects()
{
    const GLchar* vertex_shader =
        "#if defined(GL_ES)\n"
        "precision highp float;\n"
        "#endif\n"
        "uniform mat3 ProjMtx;\n"
        "attribute vec2 Position;\n"
        "attribute vec2 UV;\n"
        "attribute vec4 Color;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "  Frag_UV = UV;\n"
        "  Frag_Color = Color;\n"
        "  gl_Position = vec4(ProjMtx * vec3(Position.xy,1), 1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#if defined(GL_ES)\n"
        "precision highp float;\n"
        "#endif\n"
        "uniform sampler2D Texture;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "  gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV);\n"
        "}\n";

    g_imgui_shader = new om::shader_gl_es20(
        vertex_shader,
        fragment_shader,
        { { 0, "Position" }, { 1, "UV" }, { 2, "Color" } });

    return imgui_create_font_texture();
}

void imgui_ensure_device_objects()
{
    if (g_imgui_shader != nullptr)
    {
        return;
    }
    if (!imgui_create_device_objects())
    {
        std::cerr << "imgui_create_device_objects failed\n";
    }
}

void imgui_to_engine_render(ImDrawData* draw_data)
{
    if (draw_data == nullptr || !draw_data->Valid ||
        draw_data->CmdListsCount == 0)
    {
        return;
    }

    ImGuiIO& io       = ImGui::GetIO();
    ImVec2   fb_scale = draw_data->FramebufferScale;
    int      fb_width = static_cast<int>(draw_data->DisplaySize.x * fb_scale.x);
    int fb_height     = static_cast<int>(draw_data->DisplaySize.y * fb_scale.y);
    if (fb_width <= 0 || fb_height <= 0)
    {
        return;
    }
    draw_data->ScaleClipRects(fb_scale);

    om::mat2x3 orto_matrix =
        om::mat2x3::scale(2.0F / io.DisplaySize.x, -2.0F / io.DisplaySize.y) *
        om::mat2x3::move(om::vec2(-1.0F, 1.0F));

    g_imgui_shader->use();
    g_imgui_shader->set_uniform("ProjMtx", orto_matrix);

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list          = draw_data->CmdLists[n];
        const ImDrawIdx*  idx_buffer_offset = nullptr;

        const auto* vertex_data =
            reinterpret_cast<const om::v2*>(cmd_list->VtxBuffer.Data);
        auto vert_count = static_cast<size_t>(cmd_list->VtxBuffer.Size);

        om::vertex_buffer* vertex_buff =
            om::g_imgui_engine->create_vertex_buffer(vertex_data, vert_count);

        const std::uint16_t* indexes = cmd_list->IdxBuffer.Data;
        auto index_count = static_cast<size_t>(cmd_list->IdxBuffer.Size);

        om::index_buffer* index_buff =
            om::g_imgui_engine->create_index_buffer(indexes, index_count);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                pcmd->UserCallback(cmd_list, pcmd);
                continue;
            }

            const ImTextureID tid = pcmd->GetTexID();
            auto*             tex =
                reinterpret_cast<om::texture*>(static_cast<uintptr_t>(tid));
            auto* gl_tex = static_cast<om::texture_gl_es20*>(tex);
            g_imgui_shader->set_uniform("Texture", gl_tex);

            om::g_imgui_engine->render(vertex_buff,
                                       index_buff,
                                       tex,
                                       idx_buffer_offset,
                                       static_cast<size_t>(pcmd->ElemCount));

            idx_buffer_offset += pcmd->ElemCount;
        }
        om::g_imgui_engine->destroy_vertex_buffer(vertex_buff);
        om::g_imgui_engine->destroy_index_buffer(index_buff);
    }
}
