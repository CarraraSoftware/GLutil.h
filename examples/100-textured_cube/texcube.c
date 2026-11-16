#include <stdio.h>


#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"


#define ASSETS_PATH "./examples/100-textured_cube/assets"

const char* BALL_FILE_PATH              = ASSETS_PATH"/ball.obj";
const char* CUBE_ONLY_VERTS_FILE_PATH   = ASSETS_PATH"/tricube.obj";
const char* TEXTURED_CUBE_FILE_PATH     = ASSETS_PATH"/cube_tex.obj";
const char* MONKEY_MODEL_FILE_PATH      = ASSETS_PATH"/monkey.obj";
const char* CUBE_WITH_NORMALS_FILE_PATH = ASSETS_PATH"/normal.obj";
const char* WOODBOX_TEXTURE_FILE_PATH   = ASSETS_PATH"/texture.jpg";

const char* FLOOR_MODEL                 = ASSETS_PATH"/floortile.obj";
const char* FLOOR_TEXTURE               = ASSETS_PATH"/Cobblestone.png";


void update_camera(GLutil* glutil)
{
    const float mouse_sensiv = 0.3f;
    if ( glutil->input.mouse_button_right.pressed && glutil->input.mouse_movement.moved ) { 
        float cam_dist = Vec3f_length(Vec3f_sub(glutil->camera.pos, glutil->camera.target));
        Vec2i delta = Vec2i_sub(glutil->input.mouse_movement.pos, glutil->input.mouse_movement.last_pos);
        float theta = -mouse_sensiv * atan2f( delta.y, cam_dist );
        float phi =   -mouse_sensiv * atan2f( delta.x, cam_dist );
        glutil->camera = camera_orbit(glutil->camera, theta, phi);
    }


    Vec3f campos = glutil->camera.pos;
    Vec3f camtrg = glutil->camera.target;

#define STEP_SIZE 5.0f
    if ( glutil->input.arrow_up.pressed ) {
        Vec3f dir = Vec3f_sub(camtrg, campos);
        dir = Vec3f_normalize(dir);
        Vec3f step = Vec3f_scale(dir, STEP_SIZE);
        campos = Vec3f_add(campos, step);
    }

    if ( glutil->input.arrow_down.pressed ) {
        Vec3f dir = Vec3f_sub(camtrg, campos);
        dir = Vec3f_normalize(dir);
        Vec3f step = Vec3f_scale(dir, STEP_SIZE);
        campos = Vec3f_sub(campos, step);
    };

    glutil->camera.pos = campos;
    glutil->camera.target = camtrg;
}


const float UNIT = 1.0f;

int main(void) {
    GLutil glutil = {0};
    init(&glutil, 800, 800);
    // init(&glutil, 2560, 1440);
    set_target_fps(&glutil, 60.0f);

    // wireframe_mode();

    //fancy cube with texture and normals 
    ObjData obj = load_obj_file(CUBE_WITH_NORMALS_FILE_PATH); // loads model with default shaders and no texture
    Model cube = model_from_obj(&obj);
    if ( WOODBOX_TEXTURE_FILE_PATH ) cube.texture_id = load_texture(WOODBOX_TEXTURE_FILE_PATH);
    cube.world_pos = (Vec3f) { 
        .x =  0.0f,
        .y =  0.0f,
        .z =  0.0f,
    };
    cube.scale = UNIT;
    printf("Cube Shaders: '%s' and '%s'\n", obj.shader_program.vert_shader_filename, obj.shader_program.frag_shader_filename);

    // flashlight style light point, attached to camera
    GLColor light_clr = (GLColor) { 1.0f, 1.0f, 1.0f };
    int flashlight = add_light(&glutil, (GLutil_PointLight) {
        .pos = glutil.camera.pos,
        .clr = light_clr,
        .shine = 1.0f,
        .intensity = 10000000.0f,
    });


    // bare bones ball with only vertex data for representing the light source's position
    ObjData ball_obj = load_obj_file(BALL_FILE_PATH);
    Model ball = model_from_obj(&ball_obj);
    printf("Ball Obj Shaders: '%s' and '%s'\n", ball_obj.shader_program.vert_shader_filename, ball_obj.shader_program.frag_shader_filename);

    ball.world_pos = (Vec3f) {
        .x = 1.5f * UNIT, 
        .y = 1.5f * UNIT,
        .z = 1.5f * UNIT,  
    };
    ball.scale = UNIT / 10.0f;

    GLColor ball_clr = (GLColor) { 0.8f, 0.8f, 0.8f };

    for ( int i = 0; i < ball.geometry->vertices.len; ++i ) {
        Vertex* v = &ball.geometry->vertices.items[i];
        v->color = ball_clr;
    }


    int ball_light = add_light(&glutil, (GLutil_PointLight) {
        .pos = ball.world_pos,
        .clr = ball_clr,
        .shine = 2.0f,
        .intensity = 50000.0f,
    });


    ObjData floor_mesh     = load_obj_file(FLOOR_MODEL);
    Model   floor_tile     = model_from_obj(&floor_mesh);
    floor_tile.texture_id = load_texture(FLOOR_TEXTURE);


    glutil.camera.pos.z = UNIT/100.0f;
    glutil.camera.pos.y = UNIT/100.0f;
    glutil.camera.pos.x = UNIT/100.0f;

    for (;;) {
        uint64_t dt = begin_frame(&glutil);

        // hot reload shaders quando pressionar barra de espaço
        // if ( glutil.input.space_bar.down ) default_shaders(cube.geometry);

        update_camera(&glutil);

        if ( glutil.lights.len ) {
            get_light(&glutil, flashlight)->pos       = glutil.camera.pos;
            get_light(&glutil, flashlight)->shine     = 8.0f;
            get_light(&glutil, flashlight)->intensity = 100.0f;
        }


        Quaternion rotation = quat_from_angle_axis( 0.01, (Vec3f){ 0.0f, 1.0f, 0.0f } );
        cube.orientation = quat_mul(cube.orientation, rotation);

        Vec3f axis = { 0.0f, 1.0f, 0.0f };
        float angle = -0.01f;
        ball.world_pos = Vec3f_rotate(ball.world_pos, angle, axis);
        get_light(&glutil, ball_light)->pos = ball.world_pos;


        clear_bg(BLACK);

        const float floor_start = -10 * UNIT;
        const float floor_end   =  10 * UNIT;
        floor_tile.scale = UNIT;
        floor_tile.world_pos = (Vec3f) { 0.0f, -2.0f*UNIT, 0.0f  };

        for ( float floor_z = floor_start; floor_z < floor_end; floor_z += UNIT)
            for ( float floor_x = floor_start; floor_x < floor_end; floor_x += UNIT) {
                floor_tile.world_pos.x = floor_x;
                floor_tile.world_pos.z = floor_z;
                render_model(&glutil, &floor_tile);
            }
        // render_model(&glutil, &floor_tile);
        render_model(&glutil, &cube);
        render_model(&glutil, &ball);

        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}

