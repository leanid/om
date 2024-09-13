#version 320 es
precision mediump float;

out vec4 frag_color;

in some_block_name
{
    vec2 v_tex_coords;
} fs_in;

struct default_mat
{
    sampler2D tex_diffuse0;
};

uniform default_mat material;

/// (std140) - way to specify how to layout data
/// (shared) - way to let OpenGL driver to layout and then
/// we have to ask it by using glGetUniformIndices
/// (packed)
/// (row_major)
/// (column_major)
/// example: layout (packed, column_major)
/// example: layout (std140)
layout (std140) uniform all_not_opaque_uniforms
{                            // base alignment   // aligned offset
    bool show_z_buffer;      // 4                // 0
    bool linear_z_buffer;    // 4                // 16
    float z_near;            // 4                // 32
    float z_far;             // 4                // 48
    vec2 screen_size;        // 16               // 64
};

void main()
{
    if (show_z_buffer)
    {
        if (linear_z_buffer)
        {
            float depth = gl_FragCoord.z;
            float z = depth * 2.0 - 1.0; // back to NDC
            float z_linear = (2.0 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));
            // Because the linearized depth values range from near to far
            // most of its values will be above 1.0 and displayed as completely white
            z_linear = z_linear / z_far; // divide by far for demonstration
            frag_color = vec4(vec3(z_linear), 1.0);
        } else
        {
            frag_color = vec4(vec3(gl_FragCoord.z), 1.0);
        }
    } else
    {
        vec2 uv_pos = fs_in.v_tex_coords;
        frag_color = texture(material.tex_diffuse0, uv_pos);
    }

    if (gl_FragCoord.x < (screen_size.x / 2.0))
    {
        frag_color += vec4(0.5, 0.0, 0.0, 0.0);
    } else
    {
        frag_color += vec4(0.0, 5.0, 0.0, 0.0);
    }

    // some parts of cube facing backward so fly around and you will see
    // this triangls
    if (!gl_FrontFacing)
    {
        frag_color += vec4(0.0, 0.0, 0.75, 0.0);
    }
}
