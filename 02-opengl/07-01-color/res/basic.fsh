#version 300 es
precision mediump float;

in vec3 color;
in vec2 tex_coord;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 frag_color;

void main()
{
    frag_color = texture(texture1, tex_coord);
    vec4 col = texture(texture0, tex_coord);
    if (col.r + col.g + col.b >= 0.3)
    {
        frag_color = col;
    }
}
