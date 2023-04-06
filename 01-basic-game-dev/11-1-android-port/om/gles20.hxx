#pragma once

#include <iostream>

// #undef GL_GLEXT_PROTOTYPES
// #include <SDL2/SDL_opengl.h>
//  on Fedora linux I have nice GL ES 2 and 3 headers in system
#if defined(__ANDROID__) || defined(__LINUX__)
#define GL_GLES_PROTOTYPES 0
#include <GLES2/gl2.h>
#elif defined(__MINGW32__)
#undef GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>
// #include <GL/gl.h>
// #include <GL/glext.h>
#else
#error "add platform headers for OpenGL ES 2.0"
#endif

// we have to load all extension GL function pointers
// dynamically from OpenGL library
// so first declare function pointers for all we need
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLACTIVETEXTUREPROC            glActiveTexture;
extern PFNGLUNIFORM4FVPROC               glUniform4fv;
extern PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLBINDBUFFERPROC               glBindBuffer;
extern PFNGLBUFFERDATAPROC               glBufferData;
extern PFNGLGENBUFFERSPROC               glGenBuffers;
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLBLENDFUNCSEPARATEPROC        glBlendFuncSeparate;
extern PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate;
extern PFNGLDETACHSHADERPROC             glDetachShader;
extern PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
extern PFNGLBLENDEQUATIONPROC            glBlendEquation;
extern PFNGLISTEXTUREPROC                glIsTexture;
extern PFNGLGETERRORPROC                 glGetError;
extern PFNGLBINDTEXTUREPROC              glBindTexture;
extern PFNGLDRAWARRAYSPROC               glDrawArrays;
extern PFNGLCLEARPROC                    glClear;
extern PFNGLENABLEPROC                   glEnable;
extern PFNGLBLENDFUNCPROC                glBlendFunc;
extern PFNGLCLEARCOLORPROC               glClearColor;
extern PFNGLVIEWPORTPROC                 glViewport;
extern PFNGLGENTEXTURESPROC              glGenTextures;
extern PFNGLTEXIMAGE2DPROC               glTexImage2D;
extern PFNGLTEXPARAMETERIPROC            glTexParameteri;
extern PFNGLDELETETEXTURESPROC           glDeleteTextures;
extern PFNGLGETINTEGERVPROC              glGetIntegerv;
extern PFNGLISENABLEDPROC                glIsEnabled;
extern PFNGLDISABLEPROC                  glDisable;
extern PFNGLSCISSORPROC                  glScissor;
extern PFNGLDRAWELEMENTSPROC             glDrawElements;
extern PFNGLPIXELSTOREIPROC              glPixelStorei;

#define OM_GL_CHECK()                                                          \
    {                                                                          \
        const unsigned int err = glGetError();                                 \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            std::cerr << __FILE__ << '(' << __LINE__ << ") error: ";           \
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
