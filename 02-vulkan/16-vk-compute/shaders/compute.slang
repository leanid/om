struct compute_uniform_buffer
{
    float delta_time;
    uint particle_count;
};

ConstantBuffer<compute_uniform_buffer> ubo; // binding 0

struct particle
{
    float2 position;
    float2 velocity;
    float4 color;
};

StructuredBuffer<particle> particles_in;     // binding 1
RWStructuredBuffer<particle> particles_out; // binding 2

[shader("compute")]
[numthreads(256, 1, 1)]
void comp_main(uint3 thread_id : SV_DispatchThreadID)
{
    uint index = thread_id.x;
    if (index >= ubo.particle_count)
    {
        return;
    }

    particles_out[index].position =
        particles_in[index].position +
        particles_in[index].velocity * ubo.delta_time;
    particles_out[index].velocity = particles_in[index].velocity;
    particles_out[index].color    = particles_in[index].color;

    if ((particles_out[index].position.x <= -1.0) ||
        (particles_out[index].position.x >= 1.0))
    {
        particles_out[index].velocity.x = -particles_out[index].velocity.x;
    }
    if ((particles_out[index].position.y <= -1.0) ||
        (particles_out[index].position.y >= 1.0))
    {
        particles_out[index].velocity.y = -particles_out[index].velocity.y;
    }
}
