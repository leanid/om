#version 300 es
precision mediump float;

out vec4 frag_color;

in vec2 v_tex_coords;

uniform int current_post_process;

struct default_mat
{
    sampler2D tex_diffuse0;
};

uniform default_mat material;

const int show_framebuffer_only = 0;
const int invert_colors = 1;
const int grayscale = 2;
const int kernel_effect = 3;
const int blur_effect = 4;
const int edge_detection = 5;

void main()
{
    if (show_framebuffer_only == current_post_process)
    {
        vec3 col = texture2D(material.tex_diffuse0, v_tex_coords).rgb;
        frag_color = vec4(col, 1.0);
    } else if (invert_colors == current_post_process)
    {
        vec3 col = texture2D(material.tex_diffuse0, v_tex_coords).rgb;
        frag_color = vec4(vec3(1.0 - col), 1.0);;
    } else if (grayscale == current_post_process)
    {
        vec3 col = texture2D(material.tex_diffuse0, v_tex_coords).rgb;
        float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
        frag_color = vec4(average, average, average, 1);
    } else if (kernel_effect == current_post_process)
    {
        const float offset = 1.0 / 300.0;
        vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // top-left
            vec2( 0.0f,    offset), // top-center
            vec2( offset,  offset), // top-right
            vec2(-offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset,  0.0f),   // center-right
            vec2(-offset, -offset), // bottom-left
            vec2( 0.0f,   -offset), // bottom-center
            vec2( offset, -offset)  // bottom-right
        );

        float kernel[9] = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture2D(material.tex_diffuse0,
                                          v_tex_coords.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
        {
            col += sampleTex[i] * kernel[i];
        }

        frag_color = vec4(col, 1.0);
    } else if (blur_effect == current_post_process)
    {
        const float offset = 1.0 / 300.0;
        vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // top-left
            vec2( 0.0f,    offset), // top-center
            vec2( offset,  offset), // top-right
            vec2(-offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset,  0.0f),   // center-right
            vec2(-offset, -offset), // bottom-left
            vec2( 0.0f,   -offset), // bottom-center
            vec2( offset, -offset)  // bottom-right
        );

        float kernel[9] = float[](
            1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
            2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
            1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture2D(material.tex_diffuse0,
                                          v_tex_coords.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
        {
            col += sampleTex[i] * kernel[i];
        }

        frag_color = vec4(col, 1.0);
    } else if (edge_detection == current_post_process)
    {
        const float offset = 1.0 / 300.0;
        vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // top-left
            vec2( 0.0f,    offset), // top-center
            vec2( offset,  offset), // top-right
            vec2(-offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset,  0.0f),   // center-right
            vec2(-offset, -offset), // bottom-left
            vec2( 0.0f,   -offset), // bottom-center
            vec2( offset, -offset)  // bottom-right
        );

        float kernel[9] = float[](
            1.0, 1.0, 1.0,
            1.0, -8.0, 1.0,
            1.0, 1.0, 1.0
        );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture2D(material.tex_diffuse0,
                                          v_tex_coords.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
        {
            col += sampleTex[i] * kernel[i];
        }

        frag_color = vec4(col, 1.0);
    }
}
