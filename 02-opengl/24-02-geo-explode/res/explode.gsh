#version 320 es
layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in VS_OUT {
    vec3 normal;
    vec2 uv;
} gs_in[];

out vec2 tex_coords;

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    tex_coords = gs_in[0].uv;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    tex_coords = gs_in[1].uv;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    tex_coords = gs_in[2].uv;
    EmitVertex();
    gl_Position = explode(gl_in[0].gl_Position, normal);
    tex_coords = gs_in[0].uv;
    EmitVertex();
    EndPrimitive();
}
