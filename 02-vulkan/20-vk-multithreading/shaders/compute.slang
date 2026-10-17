struct compute_uniform_buffer
{
    float delta_time;
    uint particle_count;
};

ConstantBuffer<compute_uniform_buffer> ubo; // binding 0

// Each worker thread records its own compute command buffer and passes
// its particle range through these push constants (must match
// compute_push_constants in vulkan/render.cxx).
struct push_constants
{
    uint start_index;
    uint count;
};

[[vk::push_constant]]
push_constants pc;

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
    // only process particles within our assigned range
    if (index >= pc.count)
    {
        return;
    }

    // adjust index to start from our assigned start index
    uint global_index = pc.start_index + index;

    particles_out[global_index].position =
        particles_in[global_index].position +
        particles_in[global_index].velocity * ubo.delta_time;
    particles_out[global_index].velocity = particles_in[global_index].velocity;
    particles_out[global_index].color    = particles_in[global_index].color;

    if ((particles_out[global_index].position.x <= -1.0) ||
        (particles_out[global_index].position.x >= 1.0))
    {
        particles_out[global_index].velocity.x =
            -particles_out[global_index].velocity.x;
    }
    if ((particles_out[global_index].position.y <= -1.0) ||
        (particles_out[global_index].position.y >= 1.0))
    {
        particles_out[global_index].velocity.y =
            -particles_out[global_index].velocity.y;
    }
}
