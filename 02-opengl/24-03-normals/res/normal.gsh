#version 320 es
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec2 uv;
} gs_in[];

const float MAGNITUDE = 0.01;

uniform mat4 projection;

out vec2 tex_uv;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    tex_uv = gs_in[index].uv;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    tex_uv = gs_in[index].uv;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}
