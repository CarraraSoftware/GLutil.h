#include <stdio.h>


#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"


#define ASSETS_PATH "./examples/100-textured_cube/assets"

// const char* MODEL_FILE_PATH = ASSETS_PATH"/cube_tex.obj";
// const char* MODEL_FILE_PATH = ASSETS_PATH"/monkey.obj";
const char* MODEL_FILE_PATH   = ASSETS_PATH"/normal.obj";
const char* TEXTURE_FILE_PATH = ASSETS_PATH"/texture.jpg";

#define STEP_SIZE 5.0f

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



int main(void) {
    GLutil glutil = {0};
    init(&glutil, 800, 800);
    // init(&glutil, 2560, 1440);
    set_target_fps(&glutil, 60.0f);

    // wireframe_mode();


    ObjData obj = load_obj_file(MODEL_FILE_PATH); // loads model with default shaders and no texture
    Model cube = model_from_obj(&obj);
    if ( TEXTURE_FILE_PATH ) cube.texture_id = load_texture(TEXTURE_FILE_PATH);


    glutil.camera.pos.z = 20.0f;
    glutil.camera.pos.y = 10.0f;
    glutil.camera.pos.x = 10.0f;

    Vec3f light_clr = (Vec3f) { 1.0f, 1.0f, 1.0f };
    GLutil_PointLight l = (GLutil_PointLight) {
        .pos = glutil.camera.pos,
        .clr = light_clr,
        .shine = 1.0f,
        .intensity = 1000.0f,
    };
    vec_append(&glutil.lights, l);

    GLutil_PointLight l2 = (GLutil_PointLight) {
        .pos = (Vec3f) { 0.0f, 100.0f, 100.0f },
        .clr = (Vec3f) { 0.8f, 0.9f, 0.3f },
        .shine = 256.0f,
        .intensity = 500.0f,
    };
    vec_append(&glutil.lights, l2);

    for (;;) {
        uint64_t dt = begin_frame(&glutil);

        // hot reload shaders quando pressionar barra de espaço
        if ( glutil.input.space_bar.down ) default_shaders(cube.geometry);

        update_camera(&glutil);

        if ( glutil.lights.len ) {
            glutil.lights.items[0].pos = glutil.camera.pos;
            glutil.lights.items[0].shine = 8.0f;
            glutil.lights.items[0].intensity = 100.0f;
        }


        Quaternion rotation = quat_from_angle_axis( 0.01, (Vec3f){ 0.0f, 1.0f, 0.0f } );
        cube.orientation = quat_mul(cube.orientation, rotation);


        clear_bg(BLACK);
        render_model(&glutil, &cube);

        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}

