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

#define RAD2DEG(angle) ((angle) * 180.0f / M_PI)


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

Camera handle_mouse(GLutil* glutil, XEvent event)
{
    Camera out = *glutil->camera;

    static bool dragging = false;
    static int last_x = -1;
    static int last_y = -1;


    switch (event.type) {
        case ButtonRelease: {
            dragging = false;
            printf("Mouse Event: [RELEASED]: %d, %d\n", event.xbutton.x, event.xbutton.y);
        }; break;
        case ButtonPress: {
            dragging = true;
            last_x = event.xbutton.x;
            last_y = event.xbutton.y;
            
            printf("Mouse Event: [PRESSED]: %d, %d\n", event.xbutton.x, event.xbutton.y);
        }; break;

        case MotionNotify: {
            if ( dragging ) {
                int delta_x =  event.xmotion.x - last_x;
                int delta_y = -event.xmotion.y + last_y;

                GLutil_Vec3f dir = GLutil_Vec3f_sub(out.pos, out.target);
                dir = GLutil_Vec3f_normalize(dir);

                GLutil_Vec3f right = GLutil_Vec3f_cross(out.up, dir);
                right = GLutil_Vec3f_normalize(right);

                GLutil_Vec3f up = GLutil_Vec3f_cross(dir, right);
                up = GLutil_Vec3f_normalize(up);

                float cam_dist = Vec3f_length(Vec3f_sub(glutil->camera->pos, glutil->camera->target));

                Vec3f rotated_x = out.pos;
                Vec3f rotated_y = out.pos;

                // @TODO: aparentemente quando a câmera fica alinhada com o eixo vertical
                //        isso causa um "lookAt flip". pesquisar de quais outras formas
                //        costumam contornar essa situação além de simplesmente ignorar
                //        a rotação caso a câmera esteja muito perto do eixo.
                float zenith = fabs(RAD2DEG(Vec3f_angle_between(dir, out.up)));
                if ( 1.0f < zenith && zenith < 180.0f ) {
                    float theta = 0.1 * atan2f( delta_y, cam_dist );
                    rotated_x = Vec3f_rotate_around_axis(rotated_x, right, theta);
                    rotated_y = rotated_x;
                }
                float phi = -0.1 * atan2f( delta_x, cam_dist );
                rotated_y = Vec3f_rotate_around_axis(rotated_y, up, phi);
                out.pos = rotated_y;

                last_x = event.xmotion.x;
                last_y = event.xmotion.y;
            }
        }; break;
    }

    return out;
}

void handle_keypress(GLutil* glutil, XEvent event)
{
    float angle = 0.01f;
    float delta_z = 0.5f;
    Vec3f campos = glutil->camera->pos;
    Vec3f camtrg = glutil->camera->target;
    switch (event.xkey.keycode) {
    case RIGHT_ARROW: { }; break;
    case LEFT_ARROW: { }; break;
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
    glutil.camera->pos.x = 00.0f;
    glutil.camera->pos.y = 50.0f;
    glutil.camera->pos.z = 50.0f;
    glutil.camera->fovX = 90.0f;

    ObjData obj = GLutil_load_obj_file(OBJ_PATH);
    init_cubes(&obj);
    
    // wireframe_mode();
    for (;;) {

        while (XPending(glutil.display)) {
            XEvent event;
            XNextEvent(glutil.display, &event);
            if (event.type == KeyPress)
                handle_keypress(&glutil, event);
            else if (event.type == MotionNotify || event.type == ButtonPress || event.type == ButtonRelease)
            {
                Camera newcam = handle_mouse(&glutil, event);
                glutil.camera->pos = newcam.pos;
            }
        }

        uint64_t dt = begin_frame(&glutil);
        clear_bg(BLACK);
        for (int i = 0; i < CUBES_COUNT; ++i) render_model(&glutil, &CUBES[i]);
        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}
