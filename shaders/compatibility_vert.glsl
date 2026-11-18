#version 400 compatibility

out vec4 color;

void main()
{
    color = gl_Color;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
