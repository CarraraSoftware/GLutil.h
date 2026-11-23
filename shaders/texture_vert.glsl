#version 400 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;


// size of window

uniform vec2 window_size; // x = window_width, y = window_height
uniform vec4 texture_pos_size; // x, y = top left | z = width | w = height

out vec2 frag_pos;
out vec2 frag_tex;

void main()
{
    //-1, -1               1, -1
    // +--------------------+ screen 
    // |     x     x+w      |
    // |     +-----+        |
    // |     |     |        |
    // |     +-----+        |
    // |                    |
    // |                    |
    // |                    |
    // |                    |
    // +--------------------+
    // -1, 1               1, 1

    
    // make x and y go from 0 to 1, instead of -1 to 1
    float posx = pos.x / 2.0f + 0.5f; 
    float posy = pos.y / 2.0f + 0.5f;

    // map x=0 to onscreen_x, and x=1 to onscreen_x + onscreen_w
    float onscreen_x = texture_pos_size.x;
    float onscreen_y = texture_pos_size.y;
    float onscreen_w = texture_pos_size.z;
    float onscreen_h = texture_pos_size.w;

    float x = onscreen_x + (posx * onscreen_w);
    x = x / window_size.x;
    x = ( 2 * x - 1.0);


    float y = onscreen_y + (posy * onscreen_h);
    y = y / window_size.y;
    y = ( 2 * (1 - y) - 1.0);

    gl_Position = vec4(x, y, 0.0f, 1.0f);

    frag_pos    = vec2(x, y);
    frag_tex    = tex;
}
