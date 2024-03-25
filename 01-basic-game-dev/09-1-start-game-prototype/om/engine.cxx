#include "engine.hxx"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_stdinc.h>
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
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <thread>
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

#define OM_NO_GL_CHECK 1

#if defined(OM_NO_GL_CHECK)
#define OM_GL_CHECK()
#else
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
            }                                                                  \
            assert(false);                                                     \
        }                                                                      \
    }
#endif

namespace om
{

vec2::vec2()
    : x(0.f)
    , y(0.f)
{
}
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

matrix::matrix()
    : row0(1.0f, 0.f)
    , row1(0.f, 1.f)
    , row2(0.f, 0.f)
{
}

matrix matrix::identity()
{
    return matrix::scale(1.f);
}

matrix matrix::scale(float scale)
{
    matrix result;
    result.row0.x = scale;
    result.row1.y = scale;
    return result;
}

matrix matrix::scale(float sx, float sy)
{
    matrix r;
    r.row0.x = sx;
    r.row1.y = sy;
    return r;
}

matrix matrix::rotation(float thetha)
{
    matrix result;

    result.row0.x = std::cos(thetha);
    result.row0.y = std::sin(thetha);

    result.row1.x = -std::sin(thetha);
    result.row1.y = std::cos(thetha);

    return result;
}

matrix matrix::move(const vec2& delta)
{
    matrix r = matrix::identity();
    r.row2   = delta;
    return r;
}

vec2 operator*(const vec2& v, const matrix& m)
{
    vec2 result;
    result.x = v.x * m.row0.x + v.y * m.row0.y + m.row2.x;
    result.y = v.x * m.row1.x + v.y * m.row1.y + m.row2.y;
    return result;
}

matrix operator*(const matrix& m1, const matrix& m2)
{
    matrix r;

    r.row0.x = m1.row0.x * m2.row0.x + m1.row1.x * m2.row0.y;
    r.row1.x = m1.row0.x * m2.row1.x + m1.row1.x * m2.row1.y;
    r.row0.y = m1.row0.y * m2.row0.x + m1.row1.y * m2.row0.y;
    r.row1.y = m1.row0.y * m2.row1.x + m1.row1.y * m2.row1.y;

    r.row2.x = m1.row2.x * m2.row0.x + m1.row2.y * m2.row0.y + m2.row2.x;
    r.row2.y = m1.row2.x * m2.row1.x + m1.row2.y * m2.row1.y + m2.row2.y;

    return r;
}

texture::~texture() {}

vbo::~vbo() {}

class vertex_buffer_impl final : public vbo
{
public:
    vertex_buffer_impl(const vertex* tri, std::size_t n)
        : vertexes(n)
    {
        assert(tri != nullptr);
        std::copy_n(tri, n, begin(vertexes));
    }
    ~vertex_buffer_impl() final;

    const vertex*  data() const final { return vertexes.data(); }
    virtual size_t size() const final { return vertexes.size(); }

private:
    std::vector<vertex> vertexes;
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
        { SDL_AUDIO_F32BE, "SDL_AUDIO_F32BE" },
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

// Yes! Global variables! /////////////////////////////////////////////////////
std::ostream&            log(std::cout);
static std::atomic<bool> already_exist = false;
static SDL_Window*       window        = nullptr;
static SDL_GLContext     gl_context    = nullptr;

class shader_gl_es20;

static shader_gl_es20* shader00 = nullptr;
static shader_gl_es20* shader01 = nullptr;
static shader_gl_es20* shader02 = nullptr;
static shader_gl_es20* shader03 = nullptr;

static SDL_AudioDeviceID audio_device;
static SDL_AudioSpec     audio_device_spec;

class sound_buffer_impl;

static std::vector<sound_buffer_impl*> sounds;

static bool developer_mode     = true;
static bool reload_game        = false;
static bool continue_game_loop = true;

static int32_t exit_code = EXIT_FAILURE;
// no more Globals ////////////////////////////////////////////////////////////

#pragma pack(push, 2)
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
        // of sound and don't collide with multithreading playing
        current_index = 0;
        is_playing_   = true;
        is_looped     = (prop == effect::looped);

        SDL_ResumeAudioDevice(device);
    }
    bool is_playing() const final { return is_playing_; }
    void stop() final
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

    std::unique_ptr<uint8_t[]> tmp_buf;
    uint8_t*                   buffer;
    uint32_t                   length;
    uint32_t                   current_index = 0;
    SDL_AudioDeviceID          device;
    bool                       is_playing_ = false;
    bool                       is_looped   = false;
};
#pragma pack(pop)

sound_buffer_impl::sound_buffer_impl(std::string_view  path,
                                     SDL_AudioDeviceID device_,
                                     SDL_AudioSpec     device_audio_spec)
    : buffer(nullptr)
    , length(0)
    , device(device_)
{
    SDL_IOStream* file = SDL_IOFromFile(path.data(), "rb");
    if (file == nullptr)
    {
        throw std::runtime_error(std::string("can't open audio file: ") +
                                 path.data());
    }

    // freq, format, channels, and samples - used by SDL_LoadWAV_IO
    SDL_AudioSpec file_audio_spec;

    if (-1 ==
        SDL_LoadWAV_IO(file, SDL_TRUE, &file_audio_spec, &buffer, &length))
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
                     (static_cast<double>(file_audio_spec.channels) *
                      static_cast<double>(file_audio_spec.freq) *
                      static_cast<double>(get_sound_format_size(file_audio_spec.format)))
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

sound::~sound() {}

sound_buffer_impl::~sound_buffer_impl()
{
    if (!tmp_buf)
    {
        SDL_free(buffer);
    }
    buffer = nullptr;
    length = 0;
}

vertex_buffer_impl::~vertex_buffer_impl() {}

#pragma pack(push, 4)
class texture_gl_es20 final : public texture
{
public:
    explicit texture_gl_es20(std::string_view path);
    ~texture_gl_es20() override;

    void bind() const
    {
        GLboolean is_texture = glIsTexture(tex_handl);
        SDL_assert(is_texture);
        OM_GL_CHECK()
        glBindTexture(GL_TEXTURE_2D, tex_handl);
        OM_GL_CHECK()
    }

    std::uint32_t get_width() const final { return width; }
    std::uint32_t get_height() const final { return height; }

private:
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
        std::string_view                                      vertex_src,
        std::string_view                                      fragment_src,
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
        OM_GL_CHECK()
    }

    void set_uniform(std::string_view       uniform_name,
                     const texture_gl_es20* texture)
    {
        assert(texture != nullptr);
        const int location =
            glGetUniformLocation(program_id, uniform_name.data());
        OM_GL_CHECK()
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        unsigned int texture_unit = 0;
        glActiveTexture_(GL_TEXTURE0 + texture_unit);
        OM_GL_CHECK()

        texture->bind();

        // http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
        glUniform1i(location, static_cast<int>(0 + texture_unit));
        OM_GL_CHECK()
    }

    void set_uniform(std::string_view uniform_name, const color& c)
    {
        const int location =
            glGetUniformLocation(program_id, uniform_name.data());
        OM_GL_CHECK()
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        float values[4] = { c.get_r(), c.get_g(), c.get_b(), c.get_a() };
        glUniform4fv(location, 1, &values[0]);
        OM_GL_CHECK()
    }

    void set_uniform(std::string_view uniform_name, const matrix& m)
    {
        const int location =
            glGetUniformLocation(program_id, uniform_name.data());
        OM_GL_CHECK()
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        // OpenGL wants matrix in column major order
        // clang-format off
        float values[9] = { m.row0.x,  m.row0.y, m.row2.x,
                            m.row1.x, m.row1.y, m.row2.y,
                            0.f,      0.f,       1.f };
        // clang-format on
        glUniformMatrix3fv(location, 1, GL_FALSE, &values[0]);
        OM_GL_CHECK()
    }

private:
    GLuint compile_shader(GLenum shader_type, std::string_view src)
    {
        GLuint shader_id = glCreateShader(shader_type);
        OM_GL_CHECK()
        std::string_view vertex_shader_src = src;
        const char*      source            = vertex_shader_src.data();
        glShaderSource(shader_id, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(shader_id);
        OM_GL_CHECK()

        GLint compiled_status = 0;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(shader_id, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(shader_id);
            OM_GL_CHECK()

            std::string shader_type_name =
                shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment";
            std::cerr << "Error compiling shader(vertex)\n"
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
        OM_GL_CHECK()
        if (0 == program_id_)
        {
            std::cerr << "failed to create gl program";
            throw std::runtime_error("can't link shader");
        }

        glAttachShader(program_id_, vert_shader);
        OM_GL_CHECK()
        glAttachShader(program_id_, frag_shader);
        OM_GL_CHECK()

        // bind attribute location
        for (const auto& attr : attributes)
        {
            GLuint        loc  = std::get<0>(attr);
            const GLchar* name = std::get<1>(attr);
            glBindAttribLocation(program_id_, loc, name);
            OM_GL_CHECK()
        }

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
            std::cerr << "Error linking program:\n" << infoLog.data();
            glDeleteProgram(program_id_);
            OM_GL_CHECK()
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
    }
    return stream;
}

std::ostream& operator<<(std::ostream& out, const SDL_Version& v)
{
    out << static_cast<int>(v.major) << '.';
    out << static_cast<int>(v.minor) << '.';
    out << static_cast<int>(v.patch);
    return out;
}

std::istream& operator>>(std::istream& is, matrix& m)
{
    is >> m.row0.x;
    is >> m.row1.x;
    is >> m.row0.y;
    is >> m.row1.y;
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

std::istream& operator>>(std::istream& is, vertex& v)
{
    is >> v.pos.x;
    is >> v.pos.y;
    is >> v.uv;
    is >> v.c;
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

const std::array<bind, 8> keys{ { bind{ "up", SDLK_w, keys::up },
                                  bind{ "left", SDLK_a, keys::left },
                                  bind{ "down", SDLK_s, keys::down },
                                  bind{ "right", SDLK_d, keys::right },
                                  bind{ "button1", SDLK_LCTRL, keys::button1 },
                                  bind{ "button2", SDLK_SPACE, keys::button2 },
                                  bind{ "select", SDLK_ESCAPE, keys::select },
                                  bind{ "start", SDLK_RETURN, keys::start } } };

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

/// return seconds from initialization
float get_time_from_init()
{
    std::uint32_t ms_from_library_initialization = SDL_GetTicks();
    float         seconds = ms_from_library_initialization * 0.001f;
    return seconds;
}
/// pool event from input queue
/// return true if more events in queue
bool pool_event(event& e)
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
            e.timestamp = sdl_event.common.timestamp * 0.001;
            e.type      = om::event_type::hardware;
            return true;
        }
        else if (sdl_event.type == SDL_EVENT_KEY_DOWN ||
                 sdl_event.type == SDL_EVENT_KEY_UP)
        {
            if (developer_mode && sdl_event.key.keysym.sym == SDLK_BACKSPACE &&
                sdl_event.type == SDL_EVENT_KEY_UP)
            {
                reload_game = true;
            }

            if (check_input(sdl_event, binding))
            {
                bool is_down = sdl_event.type == SDL_EVENT_KEY_DOWN;
                e.info       = om::input_data{ binding->om_key, is_down };
                e.timestamp  = sdl_event.common.timestamp * 0.001;
                e.type       = om::event_type::input_key;
                return true;
            }
        }
    }
    return false;
}

bool is_key_down(const enum keys key)
{
    const auto it = std::find_if(
        begin(keys), end(keys), [&](const bind& b) { return b.om_key == key; });

    if (it != end(keys))
    {
        const std::uint8_t* state         = SDL_GetKeyboardState(nullptr);
        int                 sdl_scan_code = SDL_GetScancodeFromKey(it->key);
        return state[sdl_scan_code];
    }
    return false;
}

texture* create_texture(std::string_view path)
{
    return new texture_gl_es20(path);
}
void destroy_texture(texture* t)
{
    delete t;
}

vbo* create_vbo(const vertex* vertexes, std::size_t n)
{
    return new vertex_buffer_impl(vertexes, n);
}
void destroy_vbo(vbo* buffer)
{
    delete buffer;
}

sound* create_sound(std::string_view path)
{
    SDL_PauseAudioDevice(audio_device);
    sound_buffer_impl* s =
        new sound_buffer_impl(path, audio_device, audio_device_spec);
    sounds.push_back(s);
    SDL_ResumeAudioDevice(audio_device);
    return s;
}
void destroy_sound(sound* sound)
{
    delete sound;
}

static const std::array<GLenum, 6> primitive_types = { { GL_LINES,
                                                         GL_LINE_STRIP,
                                                         GL_LINE_LOOP,
                                                         GL_TRIANGLES,
                                                         GL_TRIANGLE_STRIP,
                                                         GL_TRIANGLE_FAN } };

void render(const primitives primitive_type,
            const vbo&       buff,
            const texture*   tex,
            const matrix&    m)
{
    shader03->use();
    const texture_gl_es20* texture = static_cast<const texture_gl_es20*>(tex);
    texture->bind();
    shader03->set_uniform("s_texture", texture);
    shader03->set_uniform("u_matrix", m);

    const vertex* t = buff.data();
    // positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &t->pos);
    OM_GL_CHECK()
    glEnableVertexAttribArray(0);
    OM_GL_CHECK()
    // colors
    glVertexAttribPointer(
        1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex), &t->c);
    OM_GL_CHECK()
    glEnableVertexAttribArray(1);
    OM_GL_CHECK()

    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), &t->uv);
    OM_GL_CHECK()
    glEnableVertexAttribArray(2);
    OM_GL_CHECK()

    GLsizei num_of_vertexes = static_cast<GLsizei>(buff.size());
    GLenum  priv_type = primitive_types[static_cast<uint32_t>(primitive_type)];
    glDrawArrays(priv_type, 0, num_of_vertexes);
    OM_GL_CHECK()

    glDisableVertexAttribArray(1);
    OM_GL_CHECK()
    glDisableVertexAttribArray(2);
    OM_GL_CHECK()
}

static void swap_buffers()
{
    SDL_GL_SwapWindow(window);

    glClear(GL_COLOR_BUFFER_BIT);
    OM_GL_CHECK()
}

void exit(int return_code)
{
    continue_game_loop = false;
    exit_code          = return_code;
}

static void audio_callback(void*, uint8_t*, int);

static void initialize_internal(std::string_view   title,
                                const window_mode& desired_window_mode)
{
    if (already_exist)
    {
        if (om::developer_mode && om::reload_game)
        {
            om::reload_game = false;
            return;
        }
        throw std::runtime_error("engine already initialized");
    }

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
            serr << "warning: SDL2 compiled and linked version mismatch: "
                 << compiled << " " << linked << endl;
        }

        const int init_result = SDL_Init(
            SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS |
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

        int window_size_w = static_cast<int>(desired_window_mode.width);
        int window_size_h = static_cast<int>(desired_window_mode.heigth);

        window = SDL_CreateWindow(
            title.data(), window_size_w, window_size_h, SDL_WINDOW_OPENGL);

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
        SDL_assert(result == 0);

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
        OM_GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        OM_GL_CHECK()

        glClearColor(0.f, 0.0, 0.f, 0.0f);
        OM_GL_CHECK()

        glViewport(0, 0, window_size_w, window_size_h);
        OM_GL_CHECK()

        // initialize audio
        audio_device_spec.freq     = 48000;
        audio_device_spec.format   = SDL_AUDIO_S16LE;
        audio_device_spec.channels = 2;
        // audio_device_spec.samples  = 1024; // must be power of 2
        // audio_device_spec.callback = audio_callback;
        // audio_device_spec.userdata = nullptr;

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

        int                num_audio_devices = 0;
        SDL_AudioDeviceID* currently_connected =
            SDL_GetAudioOutputDevices(&num_audio_devices);
        if (num_audio_devices > 0)
        {
            default_audio_device_name =
                SDL_GetAudioDeviceName(*currently_connected);
            for (int i = 0; i < num_audio_devices; ++i)
            {
                std::cout << "audio device #" << i << ": "
                          << SDL_GetAudioDeviceName(currently_connected[i])
                          << '\n';
            }
        }
        std::cout << std::flush;

        audio_device =
            SDL_OpenAudioDevice(*currently_connected, &audio_device_spec);

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
                      << "samples: ???" << '\n'
                      << std::flush;

            // unpause device and start audio thread
            SDL_ResumeAudioDevice(audio_device);
        }
    }

    already_exist = true;
}
static void uninitialize()
{
    if (already_exist)
    {
        /*
        SDL_PauseAudioDevice(audio_device, SDL_TRUE);
        SDL_CloseAudioDevice(audio_device);
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        */
        SDL_Quit();

        already_exist = false;
    }
}

color::color(std::uint32_t rgba_)
    : rgba(rgba_)
{
}
color::color(float r, float g, float b, float a)
{
    assert(r <= 1 && r >= 0);
    assert(g <= 1 && g >= 0);
    assert(b <= 1 && b >= 0);
    assert(a <= 1 && a >= 0);

    std::uint32_t r_ = static_cast<std::uint32_t>(r * 255);
    std::uint32_t g_ = static_cast<std::uint32_t>(g * 255);
    std::uint32_t b_ = static_cast<std::uint32_t>(b * 255);
    std::uint32_t a_ = static_cast<std::uint32_t>(a * 255);

    rgba = a_ << 24 | b_ << 16 | g_ << 8 | r_;
}

float color::get_r() const
{
    std::uint32_t r_ = (rgba & 0x000000FF) >> 0;
    return r_ / 255.f;
}
float color::get_g() const
{
    std::uint32_t g_ = (rgba & 0x0000FF00) >> 8;
    return g_ / 255.f;
}
float color::get_b() const
{
    std::uint32_t b_ = (rgba & 0x00FF0000) >> 16;
    return b_ / 255.f;
}
float color::get_a() const
{
    std::uint32_t a_ = (rgba & 0xFF000000) >> 24;
    return a_ / 255.f;
}

void color::set_r(const float r)
{
    std::uint32_t r_ = static_cast<std::uint32_t>(r * 255);
    rgba &= 0xFFFFFF00;
    rgba |= (r_ << 0);
}
void color::set_g(const float g)
{
    std::uint32_t g_ = static_cast<std::uint32_t>(g * 255);
    rgba &= 0xFFFF00FF;
    rgba |= (g_ << 8);
}
void color::set_b(const float b)
{
    std::uint32_t b_ = static_cast<std::uint32_t>(b * 255);
    rgba &= 0xFF00FFFF;
    rgba |= (b_ << 16);
}
void color::set_a(const float a)
{
    std::uint32_t a_ = static_cast<std::uint32_t>(a * 255);
    rgba &= 0x00FFFFFF;
    rgba |= a_ << 24;
}

texture_gl_es20::texture_gl_es20(std::string_view path)
    : file_path(path)
{
    std::vector<unsigned char> png_file_in_memory;
    std::ifstream              ifs(path.data(), std::ios_base::binary);
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
    OM_GL_CHECK()
    glBindTexture(GL_TEXTURE_2D, tex_handl);
    OM_GL_CHECK()

    GLint   mipmap_level = 0;
    GLint   border       = 0;
    GLsizei width        = static_cast<GLsizei>(img.width);
    GLsizei height       = static_cast<GLsizei>(img.height);
    glTexImage2D(GL_TEXTURE_2D,
                 mipmap_level,
                 GL_RGBA,
                 width,
                 height,
                 border,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 &img.raw_image[0]);
    OM_GL_CHECK()

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OM_GL_CHECK()
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OM_GL_CHECK()
}

texture_gl_es20::~texture_gl_es20()
{
    glDeleteTextures(1, &tex_handl);
    OM_GL_CHECK()
}

void audio_callback(void*, uint8_t* stream, int stream_size)
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
                SDL_MixAudioFormat(stream,
                                   current_buff,
                                   audio_device_spec.format,
                                   rest,
                                   SDL_MIX_MAXVOLUME);
                snd->current_index += rest;
            }
            else
            {
                SDL_MixAudioFormat(stream,
                                   current_buff,
                                   audio_device_spec.format,
                                   static_cast<uint32_t>(stream_size),
                                   SDL_MIX_MAXVOLUME);
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

void initialize(std::string_view title, const window_mode& desired_window_mode)
{
    initialize_internal(title, desired_window_mode);
}

window_mode get_current_window_mode()
{
    // TODO implement me
    return window_mode{ 0, 0, false };
}

lila::~lila() = default;

} // end namespace om

int initialize_and_start_main_loop()
{
    struct start
    {
        start() {}
        ~start() { om::uninitialize(); }
    } guard;

    std::vector<const char*> lib_names{
        { "libgame-09-1.dll",
          "./libgame-09-1.so",
          "./game-09-1.so",
          "./build/Debug/libgame-09-1.so",
          "./build/Debug/libgame-09-1.dll",
          "./build-Debug/libgame-09-1.so",
          "./build/RelWithDebInfo/libgame-09-1.so" }
    };

    void* so_handle = nullptr;
    auto  lib_name_it =
        std::find_if(begin(lib_names),
                     end(lib_names),
                     [&so_handle](const char* lib_name)
                     {
                         {
                             om::log << "try loading game from: " << lib_name
                                     << std::endl;
                             so_handle = SDL_LoadObject(lib_name);
                             if (so_handle == nullptr)
                             {
                                 om::log << SDL_GetError() << std::endl;
                             }
                         }
                         return so_handle != nullptr;
                     });

    if (so_handle == nullptr)
    {
        om::log << "can't load: ";
        std::copy(begin(lib_names),
                  end(lib_names),
                  std::ostream_iterator<const char*>(om::log, ", "));
        om::log << std::endl;
        return EXIT_FAILURE;
    }

    om::log << "load game from: " << *lib_name_it << std::endl;

    std::string_view om_tat_sat_func;

#if defined(__MINGW32__) || defined(__linux__) || defined(__APPLE__)
    om_tat_sat_func = "_Z10om_tat_satv";
#elif defined(_MSC_VER)
    om_tat_sat_func = "om_tat_sat::om::engine"; // TODO fix it later
#else
#error "add mangled name for your compiler"
#endif

    SDL_FunctionPointer func_addres =
        SDL_LoadFunction(so_handle, om_tat_sat_func.data());

    if (func_addres == nullptr)
    {
        om::log << "can't find " << om_tat_sat_func
                << " in so: " << *lib_name_it << std::endl;
        return EXIT_FAILURE;
    }

    std::unique_ptr<om::lila> (*f_om_tat_sat)();
    f_om_tat_sat =
        reinterpret_cast<std::unique_ptr<om::lila> (*)()>(func_addres);

start_game_again:
    std::unique_ptr<om::lila> game = f_om_tat_sat();

    if (!game)
    {
        om::log << "no game object" << std::endl;
        return EXIT_FAILURE;
    }

    using clock_timer = std::chrono::high_resolution_clock;
    using nano_sec    = std::chrono::nanoseconds;
    using milli_sec   = std::chrono::milliseconds;
    using time_point  = std::chrono::time_point<clock_timer, nano_sec>;

    clock_timer timer;

    time_point start = timer.now();

    game->on_initialize();

    while (om::continue_game_loop)
    {
        time_point end_last_frame = timer.now();
        om::event  event;

        while (pool_event(event))
        {
            game->on_event(event);
        }

        milli_sec frame_delta =
            std::chrono::duration_cast<milli_sec>(end_last_frame - start);

        if (frame_delta.count() < 15) // 1000 % 60 = 16.666 FPS
        {
            std::this_thread::yield(); // too fast, give other apps CPU time
            continue;                  // wait till more time
        }

        game->on_update(frame_delta);
        game->on_render();

        om::swap_buffers();
        start = end_last_frame;

        if (om::developer_mode && om::reload_game)
        {
            goto start_game_again;
        }
    }

    game.reset();

    return om::exit_code;
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
