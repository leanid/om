struct particle_vertex
{
    float2 position;
    float2 velocity;
    float4 color;
};

struct vertex_output
{
    float4 position : SV_Position;
    float4 frag_color;
    float point_size : SV_PointSize;
};

struct fragment_input
{
    float4 frag_color;
};

[shader("vertex")]
vertex_output main_vert(particle_vertex v)
{
    vertex_output output;
    output.position   = float4(v.position, 0.0, 1.0);
    output.frag_color = v.color;
    output.point_size = 14.0;
    return output;
}

[shader("fragment")]
float4 main_frag(fragment_input v) : SV_Target
{
    return v.frag_color;
}
