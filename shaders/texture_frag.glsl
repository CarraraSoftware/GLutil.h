#version 400 core

out vec4 FragColor;
  
in vec2 frag_pos;
in vec2 frag_tex;

uniform sampler2D the_texture;

void main()
{ 
    FragColor = texture(the_texture, frag_tex);
}
