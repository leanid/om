#version 450 // OpenGL 4.5

layout(location = 0) out vec3 frag_color;
// triangle vertex positions directly in shader code
vec3 positions[3] = vec3[]( vec3( 0.0, -0.4, 0.0 ),
                            vec3( 0.4, 0.4, 0.0 ),
                            vec3( -0.4, 0.4, 0.0 ) );

// triangle vertex colors directly in shader code
vec3 colors[3] = vec3[]( vec3( 1.0, 0.0, 0.0 ),
                         vec3( 0.0, 1.0, 0.0 ),
                         vec3( 0.0, 0.0, 1.0 ) );

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
    frag_color = colors[gl_VertexIndex];
}
