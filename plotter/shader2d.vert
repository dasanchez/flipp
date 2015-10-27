#version 330
in vec2 coord2d;
in vec3 vColor;
out vec4 vVaryingColor;
uniform mat4 projMatrix;
uniform mat4 mvMatrix;
void main(void)
{
    vVaryingColor = vec4(vColor,1.0f);
    gl_Position = projMatrix * mvMatrix * vec4(coord2d,0.0f,1.0f);
}
