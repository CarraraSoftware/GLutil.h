#version 400 core

#define quaternion vec4
#define MAX_LIGHTS 5

in vec3 vs_pos;
in vec3 vs_color;
in vec2 vs_texture_pos;
in vec3 vs_normal;
in vec4 vs_lightspace_pos[MAX_LIGHTS];

out vec4 FragColor;

uniform float t;

uniform int has_texture;        // @TODO: is there a better way of passing booleans to shaders?
uniform sampler2D the_texture;


uniform mat3 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;


uniform quaternion orientation;

uniform vec3 camera_pos;

// @NOTE: this breaks if MAX_LIGHTS gets too big. apparently there's a limit
//        to how many bytes you can pass in regular uniform.
// @TODO: refactor this thing to use a Uniform Buffer Object and 
//        check what is the role of layout(std140) in this
struct Light {
    vec3  pos;
    float shine;
    vec3  clr;
    float intensity;

    mat4      light_mat;
};

uniform int lights_count;
uniform Light lights[MAX_LIGHTS];
uniform sampler2D shadow_maps[MAX_LIGHTS];
uniform sampler2D shadow_map;

float get_shadow(vec4 ls_pos, int idx)
{
    if ( ls_pos.w < 0.0f ) {
        return 1.0f;
    }

    vec3 coords = ls_pos.xyz / ls_pos.w;
    coords = coords * 0.5f + 0.5f;

    if (coords.x < 0.0f || coords.x > 1.0f || 
        coords.y < 0.0f || coords.y > 1.0f || 
        coords.z > 1.0f) {
        return 1.0f;
    }

    float closest = texture(shadow_maps[idx], coords.xy).r;

    vec3 light_dir = normalize(lights[idx].pos - vs_pos);
    float bias = max(0.005 * (1.0 - dot(normalize(vs_normal), light_dir)), 0.0005);
    float cur = coords.z - bias;

    float shadow = cur > closest ? 0.0f : 1.0f;
    return shadow;
}


struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shineness;
};


void main()
{
#ifdef DEBUG_SHADOWMAP
    FragColor = vec4(vec3(texture(shadow_maps[0], vs_pos.xy).r), 1.0);
    return;
#endif

    Material material;
    material.ambient   = vec3(0.2, 0.2, 0.2);
    material.diffuse   = vec3(0.2, 0.2, 0.2);
    material.specular  = vec3(0.3, 0.3, 0.3);
    material.shineness = 1.0;

    float light_constant  = 1.0;
    float light_linear    = 0.0;
    float light_quadratic = 0.0;

    vec3 light_source = vec3(1.0, 1.0, 1.0);

    vec3 ambient_comp = vec3(1.0, 1.0, 1.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if ( i >= lights_count || length(vs_normal) < 0.001 ) {
            break;
        }

        vec3  light_pos       = lights[i].pos;
        vec3  light_clr       = lights[i].clr;
        float light_shine     = lights[i].shine;
        float light_intensity = lights[i].intensity;

        vec4 lightspace_pos   = vs_lightspace_pos[i];
        
        vec3 norm       = normalize(vs_normal);
        vec3 light_dir  = normalize(light_pos - vs_pos);

        float diffuse_comp = max(dot(norm, light_dir), 0.0);

        vec3 light_ray = reflect(-light_dir, norm);
        vec3 camera_dir = normalize(camera_pos - vs_pos);
        float specular_alignment = max(dot(camera_dir, light_ray), 0.0);
        float specular_comp = pow(specular_alignment, material.shineness);

        float d = distance(light_pos, vs_pos);
        float attenuation = light_intensity / (
                light_constant +
                light_linear*d +
                light_quadratic * d * d
        );

        vec3 ambient  = /* light_ambient  * */ ambient_comp  * material.ambient;
        vec3 diffuse  = /* light_diffuse  * */ diffuse_comp  * material.diffuse;
        vec3 specular = /* light_specular * */ specular_comp * material.specular;
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

        float shadow = get_shadow(lightspace_pos, i);
        light_source *=  ambient + shadow * (diffuse + specular);  
    }
    // light_source += ambient_color;

    vec3 final_color;

    if ( has_texture == 1 ) {
        final_color = texture(the_texture, vs_texture_pos).xyz;
    } else {
        final_color = vs_color;
    }

    FragColor = vec4(final_color * light_source, 1.0f);

}

