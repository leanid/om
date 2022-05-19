#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

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
   std::streambuf* clog_buf = nullptr;

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
#endif

struct context_parameters
{
  const char* name = nullptr;
  int32_t major_version = 0;
  int32_t minor_version = 0;
  int32_t profile_type = 0;
};

std::ostream& operator<<(std::ostream& out, const context_parameters& params)
{
  out << params.name << ' ' << params.major_version << '.' << params.minor_version;
  return out;
}

int main(int /*argc*/, char* /*argv*/ [])
{
    using namespace std;
    const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
    if (init_result != 0)

    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        return -1;
    }

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 640, 480, ::SDL_WINDOW_OPENGL),
        SDL_DestroyWindow);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return -1;
    }

    int r;
    context_parameters ask_context;

    using namespace std::string_literals;
    
    const char* platform_name = SDL_GetPlatform();
    if (platform_name == "Windows"s || platform_name == "Mac OS X"s)
    {
        // we want OpenGL Core 3.3 context
        ask_context.name = "OpenGL Core";
	    ask_context.major_version = 3;
	    ask_context.minor_version = 3;
	    ask_context.profile_type = SDL_GL_CONTEXT_PROFILE_CORE;
    }
    else
    {
        // we want OpenGL ES 3.0 context
        ask_context.name = "OpenGL ES";
	    ask_context.major_version = 3;
	    ask_context.minor_version = 0;
	    ask_context.profile_type = SDL_GL_CONTEXT_PROFILE_ES;
    }

    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            ask_context.profile_type);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, ask_context.major_version);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, ask_context.minor_version);
    SDL_assert_always(r == 0);

    unique_ptr<void, void (*)(void*)> gl_context(
        SDL_GL_CreateContext(window.get()), SDL_GL_DeleteContext);
    if (nullptr == gl_context)
    {
        clog << "Failed to create: " << ask_context << " error: " << SDL_GetError()
        << endl;
        SDL_Quit();
        return -1;
    }

    context_parameters got_context = ask_context;

    int result =
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &got_context.major_version);
    SDL_assert_always(result == 0);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &got_context.minor_version);
    SDL_assert_always(result == 0);

    clog << "Ask for " << ask_context << endl;
    clog << "Receive " << got_context << endl;

    bool continue_loop = true;
    while (continue_loop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (SDL_FINGERDOWN == event.type)
            {
                continue_loop = false;
                break;
            }else if (SDL_QUIT == event.type)
            {
		        continue_loop = false;
		        break;
            }
        }

        float red   = 0.f;
        float green = 1.f;
        float blue  = 0.f;
        float alpha = 0.f;

        glClearColor(red, green, blue, alpha);

        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window.get());
    }
    
    SDL_Quit();

    return 0;
}
