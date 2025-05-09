#include "engine.hxx"

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>

#include "picopng.hxx"

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
        const unsigned int err = glGetError();                                 \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            std::cerr << __FILE__ << '(' << __LINE__ - 1 << ") error: ";       \
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
            }                                                                  \
            assert(false);                                                     \
        }                                                                      \
    }

namespace om
{

vec2::vec2() = default;
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

mat2x3 mat2x3::identiry()
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
    mat2x3 r = mat2x3::identiry();
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

vbo::~vbo() = default;

class vertex_buffer_impl final : public vbo
{
public:
    vertex_buffer_impl(const tri2* tri, std::size_t n)
        : triangles(n)
    {
        assert(tri != nullptr);
        for (size_t i = 0; i < n; ++i)
        {
            triangles[i] = tri[i];
        }
    }
    ~vertex_buffer_impl() final;
    // NOLINTNEXTLINE
    const v2* data() const final { return &triangles.data()->v[0]; }
    // NOLINTNEXTLINE
    virtual size_t size() const final { return triangles.size() * 3; }

private:
    std::vector<tri2> triangles;
};

static std::string_view get_sound_format_name(uint16_t format_value)
{
    static const std::map<uint16_t, std::string_view> format = {
        { SDL_AUDIO_U8, "AUDIO_U8" },
        { SDL_AUDIO_S8, "AUDIO_S8" },
        { SDL_AUDIO_S16LE, "AUDIO_S16LSB" },
        { SDL_AUDIO_S16BE, "AUDIO_S16MSB" },
        { SDL_AUDIO_S32LE, "AUDIO_S32LSB" },
        { SDL_AUDIO_S32BE, "AUDIO_S32MSB" },
        { SDL_AUDIO_F32LE, "AUDIO_F32LSB" },
        { SDL_AUDIO_F32BE, "AUDIO_F32MSB" },
    };

    auto it = format.find(format_value);
    return it->second;
}

static std::size_t get_sound_format_size(uint16_t format_value)
{
    static const std::map<uint16_t, std::size_t> format = {
        { SDL_AUDIO_U8, 1 },    { SDL_AUDIO_S8, 1 },    { SDL_AUDIO_S16LE, 2 },
        { SDL_AUDIO_S16BE, 2 }, { SDL_AUDIO_S32LE, 4 }, { SDL_AUDIO_S32BE, 4 },
        { SDL_AUDIO_F32LE, 4 }, { SDL_AUDIO_F32BE, 4 },
    };

    auto it = format.find(format_value);
    return it->second;
}

// Yes! Global variables!
std::atomic<bool> already_exist = false;
SDL_Window*       window        = nullptr;
SDL_GLContext     gl_context    = nullptr;

class shader_gl_es20;

shader_gl_es20* shader00 = nullptr;
shader_gl_es20* shader01 = nullptr;
shader_gl_es20* shader02 = nullptr;
shader_gl_es20* shader03 = nullptr;

SDL_AudioDeviceID audio_device;
SDL_AudioSpec     audio_device_spec;

class sound_buffer_impl;

std::vector<sound_buffer_impl*> sounds;

class sound_buffer_impl final : public sound
{
public:
    sound_buffer_impl(std::string_view  path,
                      SDL_AudioDeviceID device,
                      SDL_AudioSpec     audio_spec);
    ~sound_buffer_impl() final;

    void play(const effect prop) final
    {
        // Lock callback function
        SDL_PauseAudioDevice(device);

        // here we can change properties
        // of sound and dont collade with multithreaded playing
        current_index = 0;
        is_playing_   = true;
        is_looped     = (prop == effect::looped);

        SDL_ResumeAudioDevice(device);
    }
    [[nodiscard]] bool is_playing() const final { return is_playing_; }
    void               stop() final
    {
        // Lock callback function
        SDL_PauseAudioDevice(device);

        // here we can change properties
        // of sound and dont collade with multithreaded playing
        current_index = 0;
        is_playing_   = false;
        is_looped     = false;

        // unlock callback for continue mixing of audio
        SDL_ResumeAudioDevice(device);
    }
    // NOLINTNEXTLINE
    std::unique_ptr<uint8_t[]> tmp_buf;
    uint8_t*                   buffer; // NOLINT
    uint32_t                   length        = 0;
    uint32_t                   current_index = 0;
    SDL_AudioDeviceID          device;
    bool                       is_playing_ = false;
    bool                       is_looped   = false;
};

sound_buffer_impl::sound_buffer_impl(std::string_view  path,
                                     SDL_AudioDeviceID device_,
                                     SDL_AudioSpec     device_audio_spec)
    : buffer(nullptr)
    , device(device_)
{
    SDL_IOStream* file = SDL_IOFromFile(path.data(), "rb"); // NOLINT
    if (file == nullptr)
    {
        throw std::runtime_error(std::string("can't open audio file: ") +
                                 path.data());
    }

    // freq, format, channels, and samples - used by SDL_LoadWAV_IO
    SDL_AudioSpec file_audio_spec;

    if (!SDL_LoadWAV_IO(file, true, &file_audio_spec, &buffer, &length))
    {
        throw std::runtime_error(std::string("can't load wav: ") + path.data());
    }

    std::cout << "audio format for: " << path << '\n'
              << "format: " << get_sound_format_name(file_audio_spec.format)
              << '\n'
              << "sample_size: "
              << get_sound_format_size(file_audio_spec.format) << '\n'
              << "channels: " << static_cast<uint32_t>(file_audio_spec.channels)
              << '\n'
              << "frequency: " << file_audio_spec.freq << '\n'
              << "length: " << length << '\n'
              << "time: "
              << static_cast<double>(length) /
                     static_cast<double>(
                         static_cast<size_t>(file_audio_spec.channels) *
                         file_audio_spec.freq *
                         get_sound_format_size(file_audio_spec.format))
              << "sec" << std::endl;

    if (file_audio_spec.channels != device_audio_spec.channels ||
        file_audio_spec.format != device_audio_spec.format ||
        file_audio_spec.freq != device_audio_spec.freq)
    {
        Uint8* output_bytes;
        int    output_length;

        int convert_status = SDL_ConvertAudioSamples(&file_audio_spec,
                                                     buffer,
                                                     static_cast<int>(length),
                                                     &device_audio_spec,
                                                     &output_bytes,
                                                     &output_length);
        if (0 != convert_status)
        {
            std::stringstream message;
            message << "failed to convert WAV byte stream: " << SDL_GetError();
            throw std::runtime_error(message.str());
        }

        SDL_free(buffer);
        buffer = output_bytes;
        length = static_cast<uint32_t>(output_length);
    }
    else
    {
        // no need to convert buffer, use as is
    }
}

sound::~sound() = default;

sound_buffer_impl::~sound_buffer_impl()
{
    if (!tmp_buf)
    {
        SDL_free(buffer);
    }
    buffer = nullptr;
    length = 0;
}

vertex_buffer_impl::~vertex_buffer_impl() = default;

class texture_gl_es20 final : public texture
{
public:
    explicit texture_gl_es20(std::string_view path);
    ~texture_gl_es20() override;

    void bind() const
    {
        GLboolean is_texture = glIsTexture(tex_handl);
        SDL_assert(is_texture);
        OM_GL_CHECK();
        glBindTexture(GL_TEXTURE_2D, tex_handl);
        OM_GL_CHECK();
    }

    [[nodiscard]] std::uint32_t get_width() const final { return width; }
    [[nodiscard]] std::uint32_t get_height() const final { return height; }

private:
    std::string   file_path;
    GLuint        tex_handl = 0;
    std::uint32_t width     = 0;
    std::uint32_t height    = 0;
};

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
        float values[9] = { m.col0.x,  m.col0.y, m.delta.x,
                            m.col1.x, m.col1.y, m.delta.y,
                            0.f,      0.f,       1.f };
        // clang-format on
        glUniformMatrix3fv(location, 1, GL_FALSE, &values[0]);
        OM_GL_CHECK();
    }

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
            std::cerr << "Error compiling " << shader_type_name << "\n"
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

std::ostream& operator<<(std::ostream& stream, const input_data& i)
{
    static const std::array<std::string_view, 8> key_names = { { "left",
                                                                 "right",
                                                                 "up",
                                                                 "down",
                                                                 "select",
                                                                 "start",
                                                                 "button1",
                                                                 "button2" } };

    const std::string_view& key_name = key_names[static_cast<size_t>(i.key)];

    stream << "key: " << key_name << " is_down: " << i.is_down;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const hardware_data& h)
{
    stream << "reset console: " << h.is_reset;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const event& e)
{
    switch (e.type)
    {
        case om::event_type::input_key:
            stream << std::get<om::input_data>(e.info);
            break;
        case om::event_type::hardware:
            stream << std::get<om::hardware_data>(e.info);
            break;
    };
    return stream;
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
    bind(std::string_view s, SDL_Keycode k, keys om_k)
        : name(s)
        , key(k)
        , om_key(om_k)
    {
    }

    std::string_view name;
    SDL_Keycode      key;

    om::keys om_key;
};

const std::array<bind, 8> keys{ { bind{ "up", SDLK_W, keys::up },
                                  bind{ "left", SDLK_A, keys::left },
                                  bind{ "down", SDLK_S, keys::down },
                                  bind{ "right", SDLK_D, keys::right },
                                  bind{ "button1", SDLK_LCTRL, keys::button1 },
                                  bind{ "button2", SDLK_SPACE, keys::button2 },
                                  bind{ "select", SDLK_ESCAPE, keys::select },
                                  bind{ "start", SDLK_RETURN, keys::start } } };

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

/// return seconds from initialization
float engine::get_time_from_init()
{
    std::uint64_t ms_from_library_initialization = SDL_GetTicks();
    float         seconds = ms_from_library_initialization * 0.001f; // NOLINT
    return seconds;
}
/// pool event from input queue
/// return true if more events in queue
bool engine::read_event(event& e)
{
    using namespace std;
    // collect all events from SDL
    SDL_Event sdl_event;
    if (SDL_PollEvent(&sdl_event))
    {
        const bind* binding = nullptr;

        if (sdl_event.type == SDL_EVENT_QUIT)
        {
            e.info      = om::hardware_data{ true };
            e.timestamp = sdl_event.common.timestamp * 0.001; // NOLINT
            e.type      = om::event_type::hardware;
            return true;
        }
        else if (sdl_event.type == SDL_EVENT_KEY_DOWN ||
                 sdl_event.type == SDL_EVENT_KEY_UP)
        {
            if (check_input(sdl_event, binding))
            {
                bool is_down = sdl_event.type == SDL_EVENT_KEY_DOWN;
                e.info       = om::input_data{ .key     = binding->om_key,
                                               .is_down = is_down };
                e.timestamp  = sdl_event.common.timestamp * 0.001; // NOLINT
                e.type       = om::event_type::input_key;
                return true;
            }
        }
    }
    return false;
}

bool engine::is_key_down(const enum keys key)
{
    const auto it = std::ranges::find_if(
        keys, [&](const bind& b) { return b.om_key == key; });

    if (it != end(keys))
    {
        const bool* state = SDL_GetKeyboardState(nullptr);
        SDL_Keymod  mod{};
        int         sdl_scan_code = SDL_GetScancodeFromKey(it->key, &mod);
        return state[sdl_scan_code];
    }
    return false;
}

texture* engine::create_texture(std::string_view path)
{
    return new texture_gl_es20(path);
}
void engine::destroy_texture(texture* t)
{
    delete t;
}

vbo* engine::create_vbo(const tri2* triangles, std::size_t n)
{
    return new vertex_buffer_impl(triangles, n);
}
void engine::destroy_vbo(vbo* buffer)
{
    delete buffer;
}

sound* engine::create_sound(std::string_view path)
{
    SDL_PauseAudioDevice(audio_device);
    auto* s = new sound_buffer_impl(path, audio_device, audio_device_spec);
    sounds.push_back(s);
    SDL_ResumeAudioDevice(audio_device);
    return s;
}
void engine::destroy_sound(sound* sound)
{
    delete sound;
}

void engine::render(const tri0& t, const color& c)
{
    shader00->use();
    shader00->set_uniform("u_color", c);
    // vertex coordinates
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v0), &t.v[0].pos.x);
    OM_GL_CHECK();
    glEnableVertexAttribArray(0);
    OM_GL_CHECK();

    glDrawArrays(GL_TRIANGLES, 0, 3);
    OM_GL_CHECK();
}
void engine::render(const tri1& t)
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
void engine::render(const tri2& t, texture* tex)
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]), &t.v[0].uv);
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
void engine::render(const tri2& t, texture* tex, const mat2x3& m)
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]), &t.v[0].uv);
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
void engine::render(const vbo& buff, texture* tex, const mat2x3& m)
{
    shader03->use();
    auto* texture = static_cast<texture_gl_es20*>(tex);
    texture->bind();
    shader03->set_uniform("s_texture", texture);
    shader03->set_uniform("u_matrix", m);

    const v2* t = buff.data();
    // positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), &t->pos);
    OM_GL_CHECK();
    glEnableVertexAttribArray(0);
    OM_GL_CHECK();
    // colors
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(v2), &t->c);
    OM_GL_CHECK();
    glEnableVertexAttribArray(1);
    OM_GL_CHECK();

    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(v2), &t->uv);
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
void engine::swap_buffers()
{
    SDL_GL_SwapWindow(window);

    glClear(GL_COLOR_BUFFER_BIT);
    OM_GL_CHECK();
}

void engine::exit(int return_code)
{
    std::exit(return_code);
}

static void audio_callback(void*, uint8_t*, int);

engine::engine(std::string_view)
    : log(std::cout)
{
    if (already_exist)
    {
        throw std::runtime_error("engine already exist");
    }

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

        const int init_result =
            SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS |
                     SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD);
        if (init_result != 0)
        {
            const char* err_message = SDL_GetError();
            serr << "error: failed call SDL_Init: " << err_message << endl;
            throw std::runtime_error(serr.str());
        }

        cout << "initialize om::engine" << std::endl;
        if (std::string_view("Windows") == SDL_GetPlatform())
        {
            if (!cout)
            {
#ifdef _WIN32
                AllocConsole();
#endif
                if (nullptr == std::freopen("CON", "w", stdout))
                {
                    throw std::runtime_error("can't reopen stdout");
                }
                cout.clear();
                cerr << "test" << std::endl;
                if (!cout)
                {
                    throw std::runtime_error("can't print with std::cout");
                }
            }
        }

        window = SDL_CreateWindow("title", 640, 480, SDL_WINDOW_OPENGL);

        if (window == nullptr)
        {
            const char* err_message = SDL_GetError();
            serr << "error: failed call SDL_CreateWindow: " << err_message
                 << endl;
            SDL_Quit();
            throw std::runtime_error(serr.str());
        }

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            std::string msg("can't create opengl context: ");
            msg += SDL_GetError();
            serr << msg << endl;
            throw std::runtime_error(serr.str());
        }

        int gl_major_ver = 0;
        int result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
        SDL_assert(result == 0);
        int gl_minor_ver = 0;
        result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
        assert(result == 0);

        if (gl_major_ver <= 2 && gl_minor_ver < 1)
        {
            serr << "current context opengl version: " << gl_major_ver << '.'
                 << gl_minor_ver << '\n'
                 << "need opengl version at least: 2.1\n"
                 << std::flush;
            throw std::runtime_error(serr.str());
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
            load_gl_func("glEnableVertexAttribArray",
                         glEnableVertexAttribArray);
            load_gl_func("glDisableVertexAttribArray",
                         glDisableVertexAttribArray);
            load_gl_func("glGetUniformLocation", glGetUniformLocation);
            load_gl_func("glUniform1i", glUniform1i);
            load_gl_func("glActiveTexture", glActiveTexture_);
            load_gl_func("glUniform4fv", glUniform4fv);
            load_gl_func("glUniformMatrix3fv", glUniformMatrix3fv);
        }
        catch (std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            throw;
        }

        shader00 = new shader_gl_es20(R"(
                                      attribute vec2 a_position;
                                      void main()
                                      {
                                      gl_Position = vec4(a_position, 0.0, 1.0);
                                      }
                                      )",
                                      R"(
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
        OM_GL_CHECK();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        OM_GL_CHECK();

        glClearColor(0.f, 0.0, 0.f, 0.0f);
        OM_GL_CHECK();

        glViewport(0, 0, 640, 480);
        OM_GL_CHECK();

        // initialize audio
        audio_device_spec.freq     = 48000;
        audio_device_spec.format   = SDL_AUDIO_S16LE;
        audio_device_spec.channels = 2;
        // audio_device_spec.samples  = 1024; // must be power of 2
        // audio_device_spec.callback = audio_callback;
        // audio_device_spec.userdata = this;

        const int num_audio_drivers = SDL_GetNumAudioDrivers();
        for (int i = 0; i < num_audio_drivers; ++i)
        {
            std::cout << "audio_driver #:" << i << " " << SDL_GetAudioDriver(i)
                      << '\n';
        }
        std::cout << std::flush;

        // TODO on windows 10 only "directsound" - works for me
        // if (std::string_view("Windows") == SDL_GetPlatform())
        // {
        //     const char* selected_audio_driver = SDL_GetAudioDriver(1);
        //     std::cout << "selected_audio_driver: " << selected_audio_driver
        //               << std::endl;

        //     if (0 != SDL_AudioInit(selected_audio_driver))
        //     {
        //         std::cout << "can't initialize SDL audio\n" << std::flush;
        //     }
        // }

        const char* default_audio_device_name = nullptr;

        int                      num_audio_devices = 0;
        const SDL_AudioDeviceID* audio_devices =
            SDL_GetAudioPlaybackDevices(&num_audio_devices);
        if (num_audio_devices > 0)
        {
            default_audio_device_name =
                SDL_GetAudioDeviceName(audio_devices[0]);
            for (int i = 0; i < num_audio_devices; ++i)
            {
                std::cout << "audio device #" << i << ": "
                          << SDL_GetAudioDeviceName(audio_devices[i]) << '\n';
            }
        }
        std::cout << std::flush;

        audio_device =
            SDL_OpenAudioDevice(audio_devices[0], &audio_device_spec);

        if (audio_device == 0)
        {
            std::cerr << "failed open audio device: " << SDL_GetError();
            throw std::runtime_error("audio failed");
        }
        else
        {
            std::cout << "audio device selected: " << default_audio_device_name
                      << '\n'
                      << "freq: " << audio_device_spec.freq << '\n'
                      << "format: "
                      << get_sound_format_name(audio_device_spec.format) << '\n'
                      << "channels: "
                      << static_cast<uint32_t>(audio_device_spec.channels)
                      << '\n'
                      // << "samples: " << audio_device_spec.samples << '\n'
                      << std::flush;

            // unpause device
            SDL_ResumeAudioDevice(audio_device);
        }
    }

    already_exist = true;
}
engine::~engine()
{
    if (already_exist)
    {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();

        already_exist = false;
    }
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
    return r_ / 255.f; // NOLINT
}
float color::get_g() const
{
    std::uint32_t g_ = (rgba & 0x0000FF00) >> 8;
    return g_ / 255.f; // NOLINT
}
float color::get_b() const
{
    std::uint32_t b_ = (rgba & 0x00FF0000) >> 16;
    return b_ / 255.f; // NOLINT
}
float color::get_a() const
{
    std::uint32_t a_ = (rgba & 0xFF000000) >> 24;
    return a_ / 255.f; // NOLINT
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

texture_gl_es20::texture_gl_es20(std::string_view path)
    : file_path(path)
{
    std::vector<unsigned char> png_file_in_memory;
    // NOLINTNEXTLINE
    std::ifstream ifs(path.data(), std::ios_base::binary);
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

    const om::png_image img =
        decode_png_file_from_memory(png_file_in_memory,
                                    convert_color::to_rgba32,
                                    origin_point::bottom_left);

    // if there's an error, display it
    if (img.error != 0)
    {
        std::cerr << "error: " << img.error << std::endl;
        throw std::runtime_error("can't load texture");
    }

    glGenTextures(1, &tex_handl);
    OM_GL_CHECK();
    glBindTexture(GL_TEXTURE_2D, tex_handl);
    OM_GL_CHECK();

    GLint mipmap_level = 0;
    GLint border       = 0;
    auto  width        = static_cast<GLsizei>(img.width);
    auto  height       = static_cast<GLsizei>(img.height);
    glTexImage2D(GL_TEXTURE_2D,
                 mipmap_level,
                 GL_RGBA,
                 width,
                 height,
                 border,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 &img.raw_image[0]);
    OM_GL_CHECK();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OM_GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OM_GL_CHECK();
}

texture_gl_es20::~texture_gl_es20()
{
    glDeleteTextures(1, &tex_handl);
    OM_GL_CHECK();
}

[[maybe_unused]] void audio_callback(void*, uint8_t* stream, int stream_size)
{
    // no sound default
    std::fill_n(stream, stream_size, 0);

    for (sound_buffer_impl* snd : sounds)
    {
        if (snd->is_playing_)
        {
            uint32_t rest         = snd->length - snd->current_index;
            uint8_t* current_buff = &snd->buffer[snd->current_index];

            if (rest <= static_cast<uint32_t>(stream_size))
            {
                // copy rest to buffer
                SDL_MixAudio(
                    stream, current_buff, audio_device_spec.format, rest, 1.f);
                snd->current_index += rest;
            }
            else
            {
                SDL_MixAudio(stream,
                             current_buff,
                             audio_device_spec.format,
                             static_cast<uint32_t>(stream_size),
                             1.f);
                snd->current_index += static_cast<uint32_t>(stream_size);
            }

            if (snd->current_index == snd->length)
            {
                if (snd->is_looped)
                {
                    // start from begining
                    snd->current_index = 0;
                }
                else
                {
                    snd->is_playing_ = false;
                }
            }
        }
    }
}

lila::~lila() = default;

} // end namespace om

int initialize_and_start_main_loop()
{
    om::engine engine("");

    // "game-08-2.dll" - works on windows
    // "libgame-08-2.so" - works on MacOSX and Linux
    std::string_view game_so_name("./build-Debug/libgame-08-2.so");

    // NOLINTNEXTLINE
    SDL_SharedObject* so_handle = SDL_LoadObject(game_so_name.data());
    if (so_handle == nullptr)
    {
        engine.log << "can't load " << game_so_name << ' ' << SDL_GetError()
                   << std::endl;
        return EXIT_FAILURE;
    }

    std::string_view om_tat_sat_func;

#if defined(__MINGW32__) || defined(__linux__)
    om_tat_sat_func = "_Z10om_tat_satRN2om6engineE";
#elif defined(_MSC_VER)
    om_tat_sat_func =
        "?om_tat_sat@@YA?AV?$unique_ptr@Ulila@om@@U?$default_delete@Ulila@om@@@"
        "std@@@std@@AAVengine@om@@@Z"; // TODO fix it later
#elif defined(__APPLE__)
    om_tat_sat_func = "_Z10om_tat_satRN2om6engineE";
#else
#error "add mangled name for your compiler"
#endif

    SDL_FunctionPointer func_addres =
        SDL_LoadFunction(so_handle, om_tat_sat_func.data()); // NOLINT

    if (func_addres == nullptr)
    {
        engine.log << "can't find " << om_tat_sat_func
                   << " in so: " << game_so_name << std::endl;
        return EXIT_FAILURE;
    }

    std::unique_ptr<om::lila> (*f_om_tat_sat)(om::engine&);
    f_om_tat_sat = reinterpret_cast<std::unique_ptr<om::lila> (*)(om::engine&)>(
        func_addres);

    std::unique_ptr<om::lila> game = f_om_tat_sat(engine);

    if (!game)
    {
        engine.log << "no game object" << std::endl;
        return EXIT_FAILURE;
    }

    game->on_initialize();

    while (true)
    {
        om::event event;

        while (engine.read_event(event))
        {
            game->on_event(event);
        }

        game->on_update(std::chrono::milliseconds(1));
        game->on_render();

        engine.swap_buffers();
    }

    return EXIT_SUCCESS;
}

int main(int /*argc*/, char* /*argv*/[])
{
    try
    {
        return initialize_and_start_main_loop();
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    return EXIT_FAILURE;
}
