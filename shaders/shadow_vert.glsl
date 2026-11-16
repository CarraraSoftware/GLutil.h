#version 330 core
layout (location = 0) in vec3 pos;

uniform vec3 world_pos;
uniform float scale;

uniform mat3 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main()
{
    vec3 pos1 =  world_pos + (pos * scale);
    vec4 pos0 = vec4(model_mat * pos1, 1.0f);
    gl_Position = proj_mat * view_mat * pos0;
}    
