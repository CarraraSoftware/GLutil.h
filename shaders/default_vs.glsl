#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_pos;

out vec4 vs_pos;
out vec3 vs_color;
out vec2 vs_texture_pos;
uniform float t;

const float PI = 3.14159265359;

void main() {
    // gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    // gl_Position = vec4(cos(t) * pos.x - sin(t) * pos.y , pos.y * cos(t) + sin(t)* pos.x, pos.z, 1.0);

    float delta = t * PI;

    float x = cos( t ) * pos.x - sin( t ) * pos.z;
    float y = pos.y ;
    float z = sin( t ) * pos.x + cos( t ) * pos.z;

    float freq = 2.0f;
    float rot_y =   cos( t * freq ) * y + sin( t * freq ) * z;
    float rot_z = - sin( t * freq ) * y + cos( t * freq ) * z;

    y = rot_y;
    z = rot_z;

    float rot_x = cos( t ) * x - sin( t ) * y;
          rot_y = sin( t ) * x + cos( t ) * y;

    x = rot_x;
    y = rot_y;


    float w = 1.0f;
    gl_Position = vec4(x, y, z, w);
    // gl_Position = vec4(pos.x, pos.y, pos.z, 20.0);
    vs_pos = gl_Position;
    vs_color = color;
    vs_texture_pos = tex_pos;
};







































