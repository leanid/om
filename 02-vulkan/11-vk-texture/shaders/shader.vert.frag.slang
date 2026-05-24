// Slang shader language

struct uniform_buffer
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

// if you change ordering for "ubo" and "texture" got validation layout error
ConstantBuffer<uniform_buffer> ubo; // descriptor set binding 0

Sampler2D texture; // descriptor set binding 1

struct vertex_in
{
    float3 pos;
    float3 col;
    float2 tex;
};

struct vertex_output
{
    float4 position : SV_Position;
    float3 frag_col;
    float2 tex_uv;
};

[shader("vertex")]
vertex_output main_vert(vertex_in v)
{
    vertex_output output;
    output.position =
        mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(v.pos, 1.0))));
    output.frag_col = v.col;
    output.tex_uv = v.tex;
    return output;
}

[shader("fragment")]
float4 main_frag(vertex_output v)
    : SV_Target
{
    // we need to add alpha value to result color
    return texture.Sample(v.tex_uv); // float4(v.tex_uv, 0.0, 1.0); // float4(v.frag_col, 1.0);
}
// slangc shader.frag.slang -target spirv -o shader.frag.slang.spv -profile
// spirv_1_0 -emit-spirv-directly -g2

// how I generate the spirv file:
// @note I got warning that Slang's backend support
// minimum SPIR-V 1.3, but as it still works, I ignore it
// slangc shader.vert.slang -target spirv -o shader.vert.slang.spv -profile
// spirv_1_0 If I need to debug shader I should add:
// 1. -emit-spirv-directly
// 2. -g2
// so result will be:
// slangc shader.vert.slang -target spirv -o shader.vert.slang.spv -profile
// spirv_1_0 -emit-spirv-directly -g2 But For this to work also need to set
// API_LEVEL to 1.3 in code for vulkan
