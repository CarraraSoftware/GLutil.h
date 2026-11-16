#version 330 core

#define quaternion vec4

in vec3 vs_pos;
in vec3 vs_color;
in vec2 vs_texture_pos;
in vec3 vs_normal;
in vec4 vs_lightspace_pos;

out vec4 FragColor;

uniform float t;

uniform int has_texture;        // @TODO: is there a better way of passing booleans to shaders?
uniform sampler2D the_texture;
uniform sampler2D shadow_map;

uniform mat3 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform mat4 light_mat;

uniform quaternion orientation;

uniform vec3 camera_pos;
#define MAX_LIGHTS 10

// @NOTE: this breaks if MAX_LIGHTS gets too big. apparently there's a limit
//        to how many bytes you can pass in regular uniform.
// @TODO: refactor this thing to use a Uniform Buffer Object and 
//        check what is the role of layout(std140) in this
struct Light {
    vec3  pos;
    float shine;
    vec3  clr;
    float intensity;
};

uniform int lights_count;
uniform Light lights[MAX_LIGHTS];

float get_shadow(vec4 ls_pos)
{
    vec3 coords = ls_pos.xyz / ls_pos.w;
    coords = coords / 2.0f + 0.5f;
    float closest = texture(shadow_map, coords.xy).r;
    float bias = 0.005;
    closest += bias;
    float cur = coords.z;
    float shadow = cur > closest ? 0.0f : 1.0f;
    return shadow;
}



void main()
{

    vec3 light_source = vec3(1.0, 1.0, 1.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if ( i >= lights_count || length(vs_normal) < 0.001 ) {
            break;
        }

        vec3  light_pos       = lights[i].pos;
        vec3  light_clr       = lights[i].clr;
        float light_shine     = lights[i].shine;
        float light_intensity = lights[i].intensity;
        
        vec3 norm = normalize(vs_normal);

        float d = distance(light_pos, vs_pos);
        d = d == 0 ? 0.1f : d;

        float ambient_intensity = 0.1;
        vec3  ambient_color     = vec3(1.0, 1.0, 1.0);
        ambient_color *= ambient_intensity;

        float diffuse_intensity = 0.5f; // light_intensity / (d*d);
        vec3 light_dir  = normalize(light_pos - vs_pos);
        vec3 diffuse_color = max(dot(norm, light_dir), 0.0) * light_clr;
        diffuse_color *= diffuse_intensity;

        vec3 light_ray = reflect(-light_dir, norm);
        vec3 camera_dir = normalize(camera_pos - vs_pos);

        float specular_intensity = 10.0f; // light_intensity;
        specular_intensity = specular_intensity / d;
        float shine = light_shine;
        float specular_alignment = max(dot(camera_dir, light_ray), 0.0f);
        vec3 specular_color = pow(specular_alignment, shine) * light_clr;
        specular_color *= specular_intensity;

        float shadow = get_shadow(vs_lightspace_pos);
        light_source = light_source * (ambient_color + shadow * (diffuse_color + specular_color)); 
    }

    vec3 final_color;

    if ( has_texture == 1 ) {
        final_color = texture(the_texture, vs_texture_pos).xyz;
    } else {
        final_color = vs_color;
    }

    FragColor = vec4(final_color * light_source, 1.0f);
}

