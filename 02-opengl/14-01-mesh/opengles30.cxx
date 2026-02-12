#include "opengles30.hxx"

#include <array>
#include <csignal>
#include <sstream>

#include <SDL3/SDL_assert.h>

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

const char* gl_err_to_s(GLenum err)
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

void check_gl_error(const char* file, int line)
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
        { 0, 0, "continue" },
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "break" },
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
