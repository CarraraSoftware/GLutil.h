#version 400 core
layout (location = 0) in vec3 pos;

#ifndef quaternions
#define quaternion vec4
#endif

uniform quaternion orientation;
uniform vec3 world_pos;
uniform float scale;

uniform mat3 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;


quaternion quat_from_angle_axis(float angle, vec3 axis)
{
    quaternion quat;
    float s = sin(angle);
    float c = cos(angle);
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

void main()
{
    // quaternion quat = orientation;
    // vec3 axis;
    // float angle = 2.0f * acos(quat.w);
    // float s     = sin(angle / 2.0f);
    // axis.x = quat.x / s;
    // axis.y = quat.y / s;
    // axis.z = quat.z / s;
    // vec3 rotated    = vec3_rotate( pos, angle, axis );

    vec3 rotated    = vec3_rotate_by_quat( pos, orientation );

    vec3 scaled     = rotated * scale;
    vec3 translated = world_pos + scaled;
    gl_Position     = proj_mat * view_mat * vec4(translated, 1.0f);
}
