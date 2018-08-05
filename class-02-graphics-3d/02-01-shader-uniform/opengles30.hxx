#include <iostream>

#ifdef __ANDROID__
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengles2.h>
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
