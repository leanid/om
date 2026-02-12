#pragma once

#include <iostream>

#ifdef __ANDROID__
#include <SDL3/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL3/SDL_opengles2.h>
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
// #include <GLES3/gl3.h> // TODO need for glGenVertexArrays
#endif

const char* gl_err_to_s(GLenum err);

void check_gl_error(const char* file, int line);

#define gl_check() check_gl_error(__FILE__, __LINE__);
