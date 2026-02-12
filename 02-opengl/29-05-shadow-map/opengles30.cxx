#include "opengles30.hxx"

#include <array>
#include <csignal>
#include <iostream>
#include <sstream>

#if __has_include(<winuser.h>)
#include <windows.h>
namespace gles30
{

void windows_make_process_dpi_aware() noexcept(false)
{
    const BOOL r = SetProcessDPIAware();
    if (r == 0)
    {
        throw std::runtime_error("error: SetProcessDPIAware - failed");
    }
}
} // end namespace gles30
#else
namespace gles30
{
void windows_make_process_dpi_aware() noexcept(false) {}
} // namespace gles30
#endif

#include <SDL3/SDL.h>
#ifdef __ANDROID__
#include <android/log.h>

class android_redirected_buf : public std::streambuf
{
public:
    android_redirected_buf() = default;

private:
    // This android_redirected_buf buffer has no buffer. So every character
    // "overflows" and can be put directly into the teed buffers.
    virtual int overflow(int c)
    {
        if (c == EOF)
        {
            return !EOF;
        }
        else
        {
            if (c == '\n')
            {
                // android log function add '\n' on every print itself
                __android_log_print(
                    ANDROID_LOG_ERROR, "OM", "%s", message.c_str());
                message.clear();
            }
            else
            {
                message.push_back(static_cast<char>(c));
            }
            return c;
        }
    }

    virtual int sync() { return 0; }

    std::string message;
};

struct global_redirect_handler
{
    android_redirected_buf logcat;
    std::streambuf*        clog_buf = nullptr;

    global_redirect_handler()
    {
        using namespace std;

        clog_buf = clog.rdbuf();
        clog.rdbuf(&logcat);
    }

    ~global_redirect_handler()
    {
        using namespace std;
        clog.rdbuf(clog_buf);
    }
} global_var;

#endif // __ANDROID__

namespace gles30
{

std::string_view gl_err_to_s(GLenum err)
{
    switch (err)
    {
        case GL_NO_ERROR:
            return "No error";

        case GL_INVALID_ENUM:
            return "Invalid enum";

        case GL_INVALID_VALUE:
            return "Invalid value";

        case GL_INVALID_OPERATION:
            return "Invalid operation";

        case GL_OUT_OF_MEMORY:
            return "Out of memory";

        default:
            return "Unknown error";
    }
}

void check_gl_error(std::string_view file, int line)
{
    GLenum err = glGetError();
    if (GL_NO_ERROR == err)
    {
        return;
    }

    std::stringstream ss;

    for (; GL_NO_ERROR != err; err = glGetError())
    {
        ss << file << "(" << line << ")"
           << " gl error: " << gl_err_to_s(err) << std::endl;
    }

    std::string message = ss.str();

    const std::array<SDL_MessageBoxButtonData, 2> buttons = {{
        /// .flags, .buttonid, .text
        { .flags = 0, .buttonID = 0, .text = "continue" },
        { .flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, .buttonID = 1, .text = "break" },
    }};

    SDL_MessageBoxData msg_box_data;
    msg_box_data.flags       = 0;
    msg_box_data.buttons     = buttons.data();
    msg_box_data.colorScheme = nullptr;
    msg_box_data.numbuttons  = static_cast<int>(buttons.size());
    msg_box_data.title       = "OpenGL ES 3.0 usage error";
    msg_box_data.message     = message.c_str();

    int button_id;
    int result = SDL_ShowMessageBox(&msg_box_data, &button_id);
    if (result == -1)
    {
        throw std::runtime_error("can't show message box with error:" +
                                 message);
    }
    if (button_id == 1)
    {
        // generate debug break
        std::raise(SIGABRT);
    }
}

int to_gl_filter_enum(const filter value)
{
    switch (value)
    {
        case filter::nearest:
            return GL_NEAREST;
        case filter::liner:
            return GL_LINEAR;
        case filter::nearest_mipmap_nearest:
            return GL_NEAREST_MIPMAP_NEAREST;
        case filter::nearest_mipmap_linear:
            return GL_NEAREST_MIPMAP_LINEAR;
        case filter::linear_mipmap_nearest:
            return GL_LINEAR_MIPMAP_NEAREST;
        case filter::linear_mipmap_linear:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
    throw std::runtime_error("bad filter value: " +
                             std::to_string(static_cast<int>(value)));
}

void initialize_opengles_3_2() noexcept(false)
{
    int result = gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));
    if (0 == result)
    {
        throw std::runtime_error("error: failed initialize GLES");
    }
}

bool is_desktop()
{
    using namespace std;
    string_view platform_name = SDL_GetPlatform();

    const array<string_view, 3> desktop_platforms{ "Windows"sv,
                                                   "Mac OS X"sv,
                                                   "Linux"sv };

    auto it =
        find(begin(desktop_platforms), end(desktop_platforms), platform_name);
    return it != end(desktop_platforms);
}

bool is_mobile()
{
    return !is_desktop();
}

const std::array<std::pair<std::string_view, int>, 8> z_buf_operations{
    { { "always", GL_ALWAYS },
      { "never", GL_NEVER },
      { "less", GL_LESS },
      { "equal", GL_GEQUAL },
      { "lequal", GL_LEQUAL },
      { "greater", GL_GREATER },
      { "notequal", GL_NOTEQUAL },
      { "gequal", GL_GEQUAL } }
};

const std::array<std::pair<std::string_view, int>, 8> stensil_operations{
    { { "keep", GL_KEEP },       /// The currently stored stencil value is kept.
      { "zero", GL_ZERO },       /// The stencil value is set to 0.
      { "replace", GL_REPLACE }, /// The stencil value is replaced with ref
      { "incr", GL_INCR },       /// The stencil value is increased by 1
      { "incr_wrap", GL_INCR_WRAP }, /// same as above but goto 0
      { "decr", GL_DECR },           /// decremented by 1
      { "decr_wrap", GL_DECR_WRAP }, /// same as above but restart with max FF
      { "invert", GL_INVERT } } /// Bitwise inverts the current stencil value.
};

static const char* source_to_strv(GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
    }
    return "unknown";
}

static const char* type_to_strv(GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "PUSH_GROUP";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "POP_GROUP";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
    }
    return "unknown";
}

static const char* severity_to_strv(GLenum severity)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
    }
    return "unknown";
}

// 30Kb on my system, too much for stack
static std::array<char, GL_MAX_DEBUG_MESSAGE_LENGTH> local_log_buff;

void APIENTRY callback_opengl_debug(GLenum                       source,
                                    GLenum                       type,
                                    GLuint                       id,
                                    GLenum                       severity,
                                    GLsizei                      length,
                                    const GLchar*                message,
                                    [[maybe_unused]] const void* userParam)
{
    // The memory formessageis owned and managed by the GL, and should onlybe
    // considered valid for the duration of the function call.The behavior of
    // calling any GL or window system function from within thecallback function
    // is undefined and may lead to program termination.Care must also be taken
    // in securing debug callbacks for use with asynchronousdebug output by
    // multi-threaded GL implementations.  Section 18.8 describes thisin further
    // detail.

    auto& buff{ local_log_buff };
    int   num_chars = std::snprintf(buff.data(),
                                  buff.size(),
                                  "%s %s %d %s %.*s\n",
                                  source_to_strv(source),
                                  type_to_strv(type),
                                  id,
                                  severity_to_strv(severity),
                                  length,
                                  message);

    if (num_chars > 0)
    {
        // TODO use https://en.cppreference.com/w/cpp/io/basic_osyncstream
        // to fix possible data races
        // now we use GL_DEBUG_OUTPUT_SYNCHRONOUS to garantie call in main
        // thread
        std::cerr.write(buff.data(), num_chars);
    }
}

int get_gl_constant(
    const std::array<std::pair<std::string_view, int>, 8>& operations,
    std::string_view                                       name)
{
    auto it = std::find_if(begin(operations),
                           end(operations),
                           [&name](const std::pair<std::string_view, int>& p)
                           { return p.first == name; });
    if (it == end(operations))
    {
        throw std::out_of_range(std::string("operation not found: ") +
                                std::string(name));
    }
    return it->second;
}

int get_z_buf_operation(std::string_view name)
{
    return get_gl_constant(z_buf_operations, name);
}

int get_stensil_operation(std::string_view name)
{
    return get_gl_constant(stensil_operations, name);
}

bool operator==(const gles30::context_parameters& l,
                const gles30::context_parameters& r)
{
    return std::string_view(l.name) == r.name &&
           l.major_version == r.major_version && l.minor_version &&
           r.minor_version && l.profile_type && r.profile_type;
}

bool operator!=(const gles30::context_parameters& l,
                const gles30::context_parameters& r)
{
    return !(l == r);
}

std::ostream& operator<<(std::ostream&                     out,
                         const gles30::context_parameters& params)
{
    out << params.name << ' ' << params.major_version << '.'
        << params.minor_version;
    return out;
}

void print_view_port()
{
    using namespace std;

    GLint view_port[4];
    glGetIntegerv(GL_VIEWPORT, view_port);

    clog << "view port is: x=" << view_port[0] << " y=" << view_port[1]
         << " w=" << view_port[2] << " h=" << view_port[3] << endl;
}

} // end namespace gles30
