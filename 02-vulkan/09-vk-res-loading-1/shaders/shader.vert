#version 450 // OpenGL 4.5

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

layout(location = 0) out vec3 frag_col;

void main()
{
    gl_Position = vec4(pos, 1.0);
    frag_col = col;
}
