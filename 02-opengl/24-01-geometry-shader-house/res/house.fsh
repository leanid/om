#version 320 es
precision mediump float;

in vec3 f_color;
out vec4 frag_color;

void main(void)
{
    frag_color = vec4(f_color, 1.0);
}
