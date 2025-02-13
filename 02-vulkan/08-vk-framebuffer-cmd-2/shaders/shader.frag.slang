[[shader("fragment")]]
void main(float3 frag_color, out float4 out_color: SV_Target0)
{
    // we need to add alpha value to result color
    out_color = float4(frag_color, 1.0);
}
// slangc shader.frag.slang -target spirv -o shader.frag.slang.spv -profile spirv_1_0 -emit-spirv-directly -g2
