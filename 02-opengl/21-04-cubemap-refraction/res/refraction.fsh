#version 300 es
precision mediump float;
out vec4 frag_color;

in vec3 v_normal;
in vec3 v_position;

uniform vec3 camera_pos;
struct defalt_mat
{
    samplerCube tex_cubemap0;
};

uniform defalt_mat material;

void main()
{
//    Material 	| Refractive index
//    ----------------------------
//    Air 	    | 1.00
//    Water 	| 1.33
//    Ice 	    | 1.309
//    Glass 	| 1.52
//    Diamond 	| 2.42

    float ratio = 1.0 / 1.52;
    vec3 I = normalize(v_position - camera_pos);
    vec3 R = refract(I, normalize(v_normal), ratio);
    frag_color = vec4(textureCube(material.tex_cubemap0, R).rgb, 1.0);
}
