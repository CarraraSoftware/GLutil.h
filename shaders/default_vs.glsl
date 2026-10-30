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

void main() {
    // gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
    // gl_Position = vec4(cos(t) * pos.x - sin(t) * pos.y , pos.y * cos(t) + sin(t)* pos.x, pos.z, 1.0);

    vec3 pos1 = scale * pos;
    pos1 = world_pos + pos1;
    /*
    vec3 pos1 = vec3(scale*pos.x, scale*pos.y, scale*pos.z);
    pos1 = vec3(world_pos.x + pos1.x, world_pos.y + pos1.y, world_pos.z + pos1.z);
     */

    vec3 new_pos = model_mat * pos1;
    // vec3 new_pos = pos1;

    float x = new_pos.x;
    float y = new_pos.y;
    float z = new_pos.z;


    float delta = t * PI;

    x = cos( t ) * new_pos.x - sin( t ) * new_pos.z;
    y = new_pos.y ;
    z = sin( t ) * new_pos.x + cos( t ) * new_pos.z;

    float freq = 2.0f;
    float rot_y =   cos( t * freq ) * y + sin( t * freq ) * z;
    float rot_z = - sin( t * freq ) * y + cos( t * freq ) * z;

    y = rot_y;
    z = rot_z;

    /*
    float rot_x = cos( t ) * x - sin( t ) * y;
          rot_y = sin( t ) * x + cos( t ) * y;

    x = rot_x;
    y = rot_y;
    */

    float w = 1.0f;
    vec4 pos4 = vec4(x, y, z, w);
    pos4 = view_mat * pos4;
    pos4 = proj_mat * pos4;


    gl_Position = pos4;
    // gl_Position = vec4(pos.x, pos.y, pos.z, 20.0);
    vs_pos = gl_Position;
    vs_color = color;
    vs_texture_pos = tex_pos;
};







































