#include <cstdlib>
#include <iostream>
#include <memory>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengles2.h>

int main(int /*argc*/, char* /*argv*/[])
{
    using namespace std;

    const int init_result = SDL_Init(SDL_INIT_VIDEO);
    if (init_result != 0)

    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_Init: " << err_message << endl;
        return -1;
    }
    else
    {
        atexit(SDL_Quit);
    }

    unique_ptr<SDL_Window, void (*)(SDL_Window*)> window(
        SDL_CreateWindow("title", 640, 480, ::SDL_WINDOW_OPENGL),
        SDL_DestroyWindow);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        clog << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return -1;
    }

    // we want ONLY OpenGLES 3.0 context or more
    int r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_assert_always(r == 0);
    r = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_assert_always(r == 0);

    unique_ptr<void, void (*)(void*)> gl_context(
        SDL_GL_CreateContext(window.get()), SDL_GL_DestroyContext);
    if (nullptr == gl_context)
    {
        clog << "Failed to create OpenGL Core 3.3 context: " << SDL_GetError();
        return -1;
    }

    int gl_major_ver = 0;
    int result =
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
    SDL_assert_always(result == 0);
    int gl_minor_ver = 0;
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
    SDL_assert_always(result == 0);

    clog << "Ask for OpenGL Core 3.3" << endl;
    clog << "Receive OpenGL Core " << gl_major_ver << '.' << gl_minor_ver
         << endl;

    bool continue_loop = true;
    while (continue_loop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (SDL_EVENT_QUIT == event.type)
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

    return 0;
}
