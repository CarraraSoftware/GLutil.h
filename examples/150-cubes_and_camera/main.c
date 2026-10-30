#include <X11/X.h>
#define GLUTIL_IMPLEMENTATION // inclui as implementações de todas as funções
#include "../../glutil.h"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 1200

#define CUBES_COUNT 20000

#define CUBE_MIN_SCALE  1000.0f
#define CUBE_MAX_SCALE  5000.0f

#define CUBE_MIN_POS -10000.0f
#define CUBE_MAX_POS  10000.0f


#define RIGHT_ARROW 114
#define LEFT_ARROW  113
#define DOWN_ARROW  116
#define UP_ARROW    111
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


void update_camera(GLutil* glutil, XEvent event)
{
    float angle = 0.01f;
    float delta_z = 0.5f;
    Vec3f campos = glutil->camera->pos;
    Vec3f camtrg = glutil->camera->target;
    switch (event.xkey.keycode) {
    case RIGHT_ARROW: {

        Vec3f rad = Vec3f_sub(camtrg, campos);
        rad = Vec3f_normalize(rad);

        Vec3f tang = (Vec3f) {
            .x =  -rad.z,
            .y =  0,
            .z =  rad.x,
        };

        Vec3f step = Vec3f_scale(tang, STEP_SIZE);
        campos = Vec3f_add(campos, step);

        /* weird rotation
        float new_x = campos.x * cosf(angle) - campos.z * sinf(angle);
        float new_z = campos.x * sinf(angle) + campos.z * cosf(angle);

        campos.x = new_x;
        campos.z = new_z;
        */
    }; break;
    case LEFT_ARROW: {
        Vec3f rad = Vec3f_sub(camtrg, campos);
        rad = Vec3f_normalize(rad);

        Vec3f tang = (Vec3f) {
            .x =  rad.z,
            .y =  0,
            .z =  -rad.x,
        };

        Vec3f step = Vec3f_scale(tang, STEP_SIZE);
        campos = Vec3f_add(campos, step);
        /*
        float new_x = campos.x * cosf(-angle) - campos.z * sinf(-angle);
        float new_z = campos.x * sinf(-angle) + campos.z * cosf(-angle);

        campos.x = new_x;
        campos.z = new_z;
        */
    }; break;
    case UP_ARROW: {
        Vec3f dir = Vec3f_sub(camtrg, campos);
        dir = Vec3f_normalize(dir);
        Vec3f step = Vec3f_scale(dir, STEP_SIZE);
        campos = Vec3f_add(campos, step);
    }; break;
    case DOWN_ARROW: {
        Vec3f dir = Vec3f_sub(camtrg, campos);
        dir = Vec3f_normalize(dir);
        Vec3f step = Vec3f_scale(dir, STEP_SIZE);
        campos = Vec3f_sub(campos, step);
    }; break;
    }

    glutil->camera->pos = campos;
    glutil->camera->target = camtrg;
}


int main(void) {
    GLutil glutil = {0};
    init(&glutil, WINDOW_WIDTH, WINDOW_HEIGHT);
    set_target_fps(&glutil, 999999.0f);
    srand(get_time_nanos());
    glutil.camera->pos.z = 50.0f;
    glutil.camera->fovX = 120.0f;

    ObjData obj = GLutil_load_obj_file(OBJ_PATH);
    init_cubes(&obj);
    
    // wireframe_mode();
    for (;;) {

        while (XPending(glutil.display)) {
            XEvent event;
            XNextEvent(glutil.display, &event);
            if (event.type == KeyPress)
                update_camera(&glutil, event);
        }

        uint64_t dt = begin_frame(&glutil);
        clear_bg(BLACK);
        for (int i = 0; i < CUBES_COUNT; ++i) render_model(&glutil, &CUBES[i]);
        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}
