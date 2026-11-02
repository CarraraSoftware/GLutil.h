#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_pos;

out vec4 vs_pos;
out vec3 vs_color;
out vec2 vs_texture_pos;

uniform float t;

uniform float scale;
uniform vec3  world_pos;

uniform mat3 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

const float PI = 3.14159265359;

vec3 rotate(vec3 pos)
{
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

    return vec3(x, y, z);
}

void main() {
    vec3 pos1 = scale * pos;
    pos1 = world_pos + pos1;

    vec3 new_pos = model_mat * pos1;
    // vec3 new_pos = pos1;

    float x = new_pos.x;
    float y = new_pos.y;
    float z = new_pos.z;

    // rotate();

    float w = 1.0f;
    vec4 pos4 = vec4(x, y, z, w);
    pos4 = view_mat * pos4;
    pos4 = proj_mat * pos4;


    gl_Position = pos4;
    vs_pos = gl_Position;
    vs_color = color;
    vs_texture_pos = tex_pos;
};







































