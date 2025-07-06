#version 450 // OpenGL 4.5

layout(location = 0) in vec3 frag_col;
layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(frag_col, 1.0);
}
