#include <iostream>

#ifdef __ANDROID__
#include <android/log.h>

#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>

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
                __android_log_print(ANDROID_LOG_ERROR, "OM", "%s",
                                    message.c_str());
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
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <GLES3/gl3.h> // TODO need for glGenVertexArrays
#endif

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
    while (true)
    {
        const GLenum err = glGetError();
        if (GL_NO_ERROR == err)
            break;

        std::clog << file << "(" << line << ")"
                  << " gl error: " << gl_err_to_s(err) << std::endl;
    }
}

#define gl_check() check_gl_error(__FILE__, __LINE__);
