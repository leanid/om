#version 320 es
precision mediump float;

out vec4 frag_color;

in VS_OUT {
    vec3 normal;
    vec2 uv;
} vs_out;

struct mesh_material
{
    sampler2D tex_diffuse0;
    sampler2D tex_specular0;
};

uniform mesh_material material;

uniform bool use_perspective_matrix;
uniform float near_plane;
uniform float far_plane;

// required when using a perspective projection matrix
float linearize_depth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depth_value = texture(material.tex_diffuse0, vs_out.uv).r;

    if (use_perspective_matrix)
    {
        // perspective
        frag_color = vec4(vec3(linearize_depth(depth_value) / far_plane), 1.0);
    } else
    {
        // ortogonal
        frag_color = vec4(vec3(depth_value), 1.0);
    }
}
