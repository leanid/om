#version 320 es
precision mediump float;

out vec4 frag_color;

in VS_OUT {
    vec2 uv;
} vs_out;

void main()
{
    frag_color = vec4(vs_out.uv.y, vs_out.uv.x, 0, 1);
}
