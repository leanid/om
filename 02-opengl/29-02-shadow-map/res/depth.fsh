#version 320 es

// This empty fragment shader does no processing whatsoever,
// and at the end of its run the depth buffer is updated.
// We could explicitly set the depth by uncommenting its one line,
// but this is effectively what happens behind the scene anyways.
void main()
{
    // gl_FragDepth = gl_FragCoord.z;
}
