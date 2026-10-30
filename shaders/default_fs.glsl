#version 330 core

out vec4 FragColor;
in vec4 vs_pos;
in vec3 vs_color;
in vec2 vs_texture_pos;

uniform float t;

uniform int has_texture;        // @TODO: is there a better way of passing booleans to shaders?
uniform sampler2D the_texture;

void main()
{
    // FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
       // float r = ( vs_pos.x + 1.0f) / 4.0f + ( sin(t) + 1.0f ) / 4.0f;
       // float g = ( vs_pos.y + 1.0f) / 4.0f + ( cos(t) + 1.0f ) / 4.0f; 
       // float b = ( vs_pos.z + 1.0f) / 4.0f + ( sin(t) + 1.0f ) / 4.0f;
       // float r = ( vs_pos.x + 2.0f) / 2.0f;
       // float g = ( vs_pos.y + 2.0f) / 2.0f; 
       // float b = ( vs_pos.z + 2.0f) / 2.0f;
       // float r = 0.1f;
       // float g = 0.9f;
       // float b = 0.2f;
       // FragColor = vec4( r, g, b, 1.0f );
       // FragColor = vec4( vs_color, 1.0f );
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    if (has_texture == 1) {
        FragColor = texture(the_texture, vs_texture_pos);
    } else {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}



