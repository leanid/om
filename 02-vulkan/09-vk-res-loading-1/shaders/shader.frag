#version 450 // OpenGL 4.5

// from vertex shader
layout(location = 0) in vec3 frag_color;
// output color for framebuffer(blending)
layout(location = 0) out vec4 out_color;

void main()
{
    // we need to add alpha value to result color
    out_color = vec4(frag_color, 1.0);
}
