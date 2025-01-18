// Slang shader language

// triangle vertex positions directly in shader code
static const float3 positions[3] = { { 0.0, -0.4, 0.0 },
                                     { 0.4, 0.4, 0.0 },
                                     { -0.4, 0.4, 0.0 } };

// triangle vertex colors directly in shader code
static const float3 colors[3] = { { 1.0, 0.0, 0.0 },
                                  { 0.0, 1.0, 0.0 },
                                  { 0.0, 0.0, 1.0 } };

[[shader("vertex")]]
void main(uint       vertex_id : SV_VertexID,
          out float4 position : SV_Position,
          out float3 color)
{
    position = float4(positions[vertex_id], 1.0);
    color    = colors[vertex_id];
}
// how I generate the spirv file:
// @note I got warning that Slang's backend support
// minimum SPIR-V 1.3, but as it still works, I ignore it
//slangc shader.vert.slang -target spirv -o shader.vert.slang.spv -profile spirv_1_0
// If I need to debug shader I should add:
// 1. -emit-spirv-directly
// 2. -g2
// so result will be:
// slangc shader.vert.slang -target spirv -o shader.vert.slang.spv -profile spirv_1_0 -emit-spirv-directly -g2
// But For this to work also need to set API_LEVEL to 1.3 in code for vulkan
