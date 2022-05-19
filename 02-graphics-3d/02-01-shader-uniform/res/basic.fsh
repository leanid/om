#version 300 es
precision mediump float;
uniform float sin_value;
out vec4 frag_color;
void main()
{
    frag_color = vec4(1.0f, sin_value, 0.2f, 1.0f);
}
