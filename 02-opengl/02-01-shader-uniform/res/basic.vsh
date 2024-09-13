#version 300 es
layout (location = 0) in vec4 a_position;
void main()
{
    gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0);
}
