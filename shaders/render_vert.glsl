#version 400 core

#ifndef quaternions
#define quaternion vec4
#endif

#define MAX_LIGHTS 5

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_pos;
layout (location = 3) in vec3 normal;

out vec3 vs_pos;
out vec3 vs_color;
out vec2 vs_texture_pos;
out vec3 vs_normal;
out vec3 vs_camera_pos;
out vec4 vs_lightspace_pos[MAX_LIGHTS];

uniform float t;
uniform float scale;
uniform vec3  world_pos;
uniform quaternion orientation;

uniform mat3 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

uniform vec3 camera_pos;

const float PI = 3.14159265359;

struct Light {
    vec3  pos;
    float shine;
    vec3  clr;
    float intensity;

    mat4  light_mat;  // proj * view from light's pov
};

uniform int lights_count;
uniform Light lights[MAX_LIGHTS];



#ifndef quaternions
#define quaternion vec4
#endif

quaternion quat_from_angle_axis(float angle, vec3 axis)
{
    quaternion quat;
    float s = sin(angle / 2.0f);
    float c = cos(angle / 2.0f);
    quat.w = c;
    quat.x = axis.x * s;
    quat.y = axis.y * s;
    quat.z = axis.z * s;
    return quat;
}

quaternion quat_from_vec3(vec3 v)
{
    return quaternion(v, 0.0f);
}

float quat_sqlength(quaternion q)
{
    // return q*q
    return (q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

quaternion quat_conjugate(quaternion q)
{
    return quaternion(-q.x, -q.y, -q.z, q.w);
}

quaternion quat_scale(quaternion q, float s)
{
    return quaternion( q.x * s, q.y * s, q.z * s, q.w * s );
}

quaternion quat_div_scalar(quaternion v, float d)
{
    if ( d == 0.0f ) return v;
    float s = 1.0f / d;
    return quat_scale(v, s);
}

quaternion quat_mul(quaternion a_, quaternion b_)
{
// @FROM: https://en.wikipedia.org/wiki/quaternions_and_spatial_rotation#quaternions
    float a = a_.w;
    float b = a_.x;
    float c = a_.y;
    float d = a_.z;
    float e = b_.w;
    float f = b_.x;
    float g = b_.y;
    float h = b_.z;

    quaternion q; // = {0};

    q.w = (a*e - b*f - c*g - d*h);
    q.x = (a*f + b*e + c*h - d*g);
    q.y = (a*g - b*h + c*e + d*f);
    q.z = (a*h + b*g - c*f + d*e);
    return q;
}

quaternion quat_inverse(quaternion q)
{
    return quat_conjugate(q) / quat_sqlength(q);
}

vec3 vec3_rotate_by_quat(vec3 v, quaternion q)
{
    quaternion vquat    = quat_from_vec3(v);
    quaternion qinv     = quat_inverse(q);
    quaternion left_mul = quat_mul(q, vquat);
    quaternion result   = quat_mul(left_mul, qinv);
    return result.xyz;
}

vec3 vec3_rotate(vec3 v, float angle, vec3 axis)
{
    quaternion q = quat_from_angle_axis(angle/2.0f, axis);
    return vec3_rotate_by_quat(v, q);
}


void main() {
    vec3 normal0 = vec3_rotate_by_quat( normal/scale, orientation );

    vec3 rotated    = vec3_rotate_by_quat( pos, orientation );
    vec3 scaled     = rotated * scale;
    vec3 translated = world_pos + scaled;

    gl_Position = proj_mat * view_mat *  vec4(translated, 1.0f);

    vs_pos             = translated; // world position do POV da camera
    vs_color           = color;
    vs_texture_pos     = tex_pos;
    vs_normal          = normal0;
    vs_camera_pos      = camera_pos;

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if ( i >= lights_count ) {
            break;
        }
        vec4 light_pos = lights[i].light_mat * vec4(translated, 1.0f);
        vs_lightspace_pos[i] = light_pos; // world position do POV de lights[i]
    }
};
  
