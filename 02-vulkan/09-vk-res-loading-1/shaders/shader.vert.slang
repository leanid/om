// Slang shader language

[[shader("vertex")]]
void main(in float3 pos,
          in float3 col,
          out float4 position : SV_Position,
          out float3 frag_col)
{
    position = float4(pos, 1.0);
    frag_col = col;
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
