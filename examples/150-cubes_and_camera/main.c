#include <X11/X.h>


#define GLUTIL_IMPLEMENTATION // inclui as implementações de todas as funções
#include "../../glutil.h"

#define WIRED 0

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 1200

#define CUBES_COUNT 20000

#define CUBE_MIN_SCALE  1000.0f
#define CUBE_MAX_SCALE  5000.0f

#define CUBE_MIN_POS -10000.0f
#define CUBE_MAX_POS  10000.0f


#define STEP_SIZE 2.0f


Model CUBES[CUBES_COUNT] = {0};

#define OBJ_PATH  "./examples/150-cubes_and_camera/assets/cube_tex.obj"
#define TEXTURE_1 "./examples/150-cubes_and_camera/assets/texture1.jpg"
#define TEXTURE_2 "./examples/150-cubes_and_camera/assets/texture2.png"



void init_cubes(ObjData* obj)
{
    GLuint texture_1 = load_texture(TEXTURE_1);
    GLuint texture_2 = load_texture(TEXTURE_2);
    for (int i = 0; i < CUBES_COUNT; ++i) {
        float r = RAND(0.0f, 1.0f);

        GLuint texture;
        if (r < 0.5f) texture = texture_1;
        else          texture = texture_2;

        CUBES[i] = (Model) {
            .scale      = RAND(CUBE_MIN_SCALE, CUBE_MAX_SCALE),
            .world_pos  = { 
                .x = RAND(CUBE_MIN_POS, CUBE_MAX_POS), 
                .y = RAND(CUBE_MIN_POS, CUBE_MAX_POS),  
                .z = RAND(CUBE_MIN_POS, CUBE_MAX_POS), 
            },
            .geometry   = obj,
            .texture_id = texture,
        };
    }
}


void update_camera_position(GLutil* glutil)
{
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


void move_camera_from_mouse(GLutil* glutil)
{
    const float mouse_sensiv = 0.3f;
    if ( glutil->input.mouse_button_right.pressed && glutil->input.mouse_movement.moved ) { 
        float cam_dist = Vec3f_length(Vec3f_sub(glutil->camera.pos, glutil->camera.target));
        Vec2i delta = Vec2i_sub(glutil->input.mouse_movement.pos, glutil->input.mouse_movement.last_pos);
        float theta = -mouse_sensiv * atan2f( delta.y, cam_dist );
        float phi =   -mouse_sensiv * atan2f( delta.x, cam_dist );
        glutil->camera = camera_orbit(glutil->camera, theta, phi);
    }
}


int main(void) {
    GLutil glutil = {0};
    init(&glutil, WINDOW_WIDTH, WINDOW_HEIGHT);
    unlimited_fps(&glutil);
    srand(get_time_nanos());

    glutil.camera.pos.x = 00.0f;
    glutil.camera.pos.y = 50.0f;
    glutil.camera.pos.z = 50.0f;
    glutil.camera.fovX = 90.0f;


    /*
    printf("Size of Input: %ld\n", sizeof(Input));
    return 0;
    */
    ObjData obj = GLutil_load_obj_file(OBJ_PATH);
    init_cubes(&obj);
    
#if WIRED
    wireframe_mode();
#endif
    for (;;) {
        uint64_t dt = begin_frame(&glutil);
        {
            clear_bg(BLACK);
            move_camera_from_mouse(&glutil);
            update_camera_position(&glutil);
            for (int i = 0; i < CUBES_COUNT; ++i) render_model(&glutil, &CUBES[i]);
        }
        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}
