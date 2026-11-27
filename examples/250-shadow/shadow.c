#include <stdio.h>
#include <stdarg.h>

#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"

#define TWOBALLS 1


#define WINWIDTH  800
#define WINHEIGHT 800


#define ASSETS_PATH "./examples/250-shadow/assets"



int main(void)
{
    GLutil g = {};
    init(&g, WINWIDTH, WINHEIGHT);
    set_target_fps(&g, 60.0f);

    GLutil_camera_first_person(&g);

    g.camera.pos.z = 5.0f;
    g.camera.pos.y = 3.0f;

    /********************** FLOOR  ********************************************/
    ObjData ground_obj = load_obj_file( ASSETS_PATH "/floortile.obj"); 
    Model ground     = model_from_obj(&ground_obj);
        ground.texture_id  = texture_from_image( ASSETS_PATH "/floortile.png");
        ground.scale       = +5.0f;
        ground.world_pos.y = -1.5f;
    /**************************************************************************/


    /********************** CUBE **********************************************/
    ObjData cube_obj = load_obj_file( ASSETS_PATH "/pinkube.obj");
    Model cube = model_from_obj(&cube_obj);
        cube.texture_id  = texture_from_image(ASSETS_PATH"/pinkube.jpg");
        cube.world_pos.z = +0.0f;
        cube.world_pos.y = +0.5f;
        cube.scale       = +1.0f;
    /**************************************************************************/


    /******************* LIGHT BALL *******************************************/
    ObjData ball_obj = load_obj_file(ASSETS_PATH"/ball.obj");
    Model   ball     = model_from_obj(&ball_obj);
    ball.world_pos   = (Vec3f) {
        .x =  5.0f,
        .y =  7.5f,
        .z =  5.0f,
    };
    ball.scale = 1.0f;

    int light_id = add_light(&g, (PointLight) {
        .intensity =  3.0f,  
        .shine     =  2.0f,  
        .pos       =  ball.world_pos,
        .clr       =  (GLColor) { 1.0f, 1.0f, 1.0f },  
    });
    /**************************************************************************/


    /********** ANOTHER LIGHT BALL **********/
#if (TWOBALLS)
    Model ball2 = model_from_obj(&ball_obj);
    ball2.world_pos   = (Vec3f) {
        .x = -2.0f,
        .y =  2.5f,
        .z = -2.0f,
    };
    ball2.scale = 0.2f;

    int light_id2 = add_light(&g, (PointLight) {
        .intensity =  1.0f,  
        .shine     =  4.0f,  
        .pos       =  ball2.world_pos,
        .clr       =  (GLColor) { 1.0f, 1.0f, 1.0f },  
    });
#endif 
    /*******************************************************/


    Model* models[] = {
        &cube, 
        &ground, 
        &ball,

#if (TWOBALLS) 
        &ball2,
#endif

    };

    const int models_count = sizeof(models) / sizeof(*models);


    for (;;) {
        begin_frame(&g);
        float t = GLutil_get_time_secs(&g);

/************************** UPDATE ********************************************/
            camera_update(&g);
            float delta_angle = 0.01;
            Vec3f axis = (Vec3f) { 0.0f, 1.0f, 0.0f };

            for ( int i = 0; i < models_count; ++i) {
                // update_model(); // maybe function pointer in the Model struct???? 
            }

            if ( g.input.space_bar.down ) {
                shader_reload(shadow_shaders());
                shader_reload(render_shaders());
            }

            cube.orientation = quat_mul(cube.orientation, quat_from_angle_axis(2 * delta_angle, axis));

            ball.world_pos = Vec3f_rotate( ball.world_pos, delta_angle, axis);
            PointLight* l = get_light(&g, light_id);
            l->pos = ball.world_pos;

#if (TWOBALLS)
            ball2.world_pos = Vec3f_rotate( ball2.world_pos, -delta_angle, axis);
            PointLight* l2 = get_light(&g, light_id2);
            l2->pos = ball2.world_pos;
#endif
/******************************************************************************/


/************************** SHADOWMAP *****************************************/
            compute_shadow_maps(&g, 2, (Model*[]){ &cube, &ground });
/******************************************************************************/
            

/************************* RENDER *********************************************/
            clear_bg(BLACK);
            render_model(&g, &cube);
            render_model(&g, &ball);
            render_model(&g, &ground);

#if (TWOBALLS)
            render_model(&g, &ball2);
#endif
/******************************************************************************/

        end_frame(&g);
    }


    printf("hello world\n");
    return 0;
}
