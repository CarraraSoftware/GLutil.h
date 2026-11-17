#include <stdio.h>
#include <stdarg.h>

#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"

#define TWOBALLS 1


#define WINWIDTH  800
#define WINHEIGHT 800

void update_camera(GLutil* glutil)
{
    const float mouse_sensiv = 0.03f;
    if ( glutil->input.mouse_button_right.pressed && glutil->input.mouse_movement.moved ) { 
        float cam_dist = Vec3f_length(Vec3f_sub(glutil->camera.pos, glutil->camera.target));
        Vec2i delta = Vec2i_sub(glutil->input.mouse_movement.pos, glutil->input.mouse_movement.last_pos);
        float theta = -mouse_sensiv * atan2f( delta.y, cam_dist );
        float phi =   -mouse_sensiv * atan2f( delta.x, cam_dist );
        glutil->camera = camera_orbit(glutil->camera, theta, phi);
    }

    Vec3f campos = glutil->camera.pos;
    Vec3f camtrg = glutil->camera.target;

#define STEP_SIZE 0.5f
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

#define ASSETS_PATH "./examples/250-shadow/assets"



int main(void)
{
    GLutil g = {};
    init(&g, WINWIDTH, WINHEIGHT);
    set_target_fps(&g, 60.0f);

    g.camera.pos.z = 3.0f;
    g.camera.pos.y = 2.0f;

    GLutil_ShaderProgram shadow_shader = shader_new("./shaders/shadow_vert.glsl", "./shaders/shadow_frag.glsl");

    /********** FLOOR  ****************/
    ObjData ground_obj = load_obj_file( ASSETS_PATH "/floortile.obj"); 
    Model   ground     = model_from_obj(&ground_obj);
    // ground.texture_id  = load_texture( ASSETS_PATH "/darkfloor.png");
    ground.texture_id  = load_texture( ASSETS_PATH "/floortile.png");
    ground.scale       = 5.0f;
    ground.world_pos.y = -1.0f;
    /*******************************************************/


    /********** CUBE ****************/
    ObjData cube_obj = load_obj_file( ASSETS_PATH "/pinkube.obj");
    Model   cube = model_from_obj(&cube_obj);
    cube.texture_id = load_texture(ASSETS_PATH"/pinkube.jpg");
    cube.world_pos.z =  0.0f;
    cube.world_pos.y =  0.5f;
    cube.scale       =  0.25f;
    /*******************************************************/


    /********** LIGHT BALL **********/
    ObjData ball_obj = load_obj_file(ASSETS_PATH"/ball.obj");
    Model   ball     = model_from_obj(&ball_obj);
    ball.world_pos   = (Vec3f) {
        .x =  1.0f,
        .y =  1.5f,
        .z =  1.0f,
    };
    ball.scale = 0.05f;

    int light_id = add_light(&g, (PointLight) {
        .intensity =  0.0f,  
        .shine     =  1.0f,  
        .pos       =  ball.world_pos,
        .clr       =  (GLColor) { 1.0f, 1.0f, 1.0f },  
    });
    /*******************************************************/


#if (TWOBALLS)
    /********** ANOTHER LIGHT BALL **********/
    Model ball2 = model_from_obj(&ball_obj);
    ball2.world_pos   = (Vec3f) {
        .x = -1.0f,
        .y =  1.5f,
        .z = -1.0f,
    };
    ball2.scale = 0.05f;

    int light_id2 = add_light(&g, (PointLight) {
        .intensity =  10000.0f,  
        .shine     =  2.0f,  
        .pos       =  ball2.world_pos,
        .clr       =  (GLColor) { 1.0f, 1.0f, 1.0f },  
    });
    /*******************************************************/
#endif 


    for (;;) {
        begin_frame(&g);
        float t = GLutil_get_time_secs(&g);

            /**************** UPDATE ******************************************/
            update_camera(&g);
            float delta_angle = 0.01;
            Vec3f axis = (Vec3f) { 0.0f, 1.0f, 0.0f };

            ball.world_pos = Vec3f_rotate( ball.world_pos, delta_angle, axis);
            PointLight* l = get_light(&g, light_id);
            l->pos = ball.world_pos;

#if (TWOBALLS)
            ball2.world_pos = Vec3f_rotate( ball2.world_pos, -delta_angle, axis);
            PointLight* l2 = get_light(&g, light_id2);
            l2->pos = ball2.world_pos;
#endif
            /******************************************************************/


            compute_shadow_maps(&g, shadow_shader, 2, (Model*[]){ &cube, &ground });
            

            /*************** CAMERA RENDER ************************************/
            clear_bg(BLACK);
            render_model(&g, &cube);
            render_model(&g, &ball);
            render_model(&g, &ground);

#if (TWOBALLS)
            render_model(&g, &ball2);
#endif
            /******************************************************************/

        end_frame(&g);
    }


    printf("hello world\n");
    return 0;
}
