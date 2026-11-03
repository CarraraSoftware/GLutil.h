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

Vec3f camera_direction(Camera* camera)
{
    GLutil_Vec3f dir = GLutil_Vec3f_sub(camera->target, camera->pos);
    return GLutil_Vec3f_normalize(dir);
}

Vec3f camera_right(Camera* camera)
{
    GLutil_Vec3f dir = camera_direction(camera);
    GLutil_Vec3f right = GLutil_Vec3f_cross(dir, camera->up);
    return GLutil_Vec3f_normalize(right);
}

Vec3f camera_up(Camera* camera)
{
    GLutil_Vec3f dir = camera_direction(camera);
    GLutil_Vec3f right = camera_right(camera);
    GLutil_Vec3f up = Vec3f_normalize(Vec3f_cross(right, dir));
    return up;
}

typedef struct {
    Vec3f front, right, up;
} _3DBasis;

_3DBasis camera_basis(Camera* camera)
{
    GLutil_Vec3f dir = camera_direction(camera);                // aponta da camera pra alvo, atua como eixo z local 
    GLutil_Vec3f right = camera_right(camera);                  // perpendicular a dir e ao 'up' global, atua como x local
    GLutil_Vec3f up = Vec3f_normalize(Vec3f_cross(right, dir)); // perpendicular ao dois primeiros, atua como y local
    return (_3DBasis) {
        .front = dir, .right = right, .up = up,
    };
}


Camera handle_mouse(GLutil* glutil)
{
    Camera out  = glutil->camera;
    Input input = glutil->input;


    printf( "(%ld) HANDLE MOUSE:  input.mouse_button_right.pressed = %s && input.mouse_movement.moved = %s\n",
        glutil->cur_t_nanos,
        input.mouse_button_right.pressed ? "true" : "false",
        input.mouse_movement.moved ? "true" : "false"
    );
    if ( input.mouse_button_right.pressed && input.mouse_movement.moved ) { 
        

        Vec2i delta = Vec2i_sub(input.mouse_movement.pos, input.mouse_movement.last_pos);

        float cam_dist = Vec3f_length(Vec3f_sub(glutil->camera.pos, glutil->camera.target));
        _3DBasis cam_basis = camera_basis(&out);

        Vec3f rotated_x = out.pos;
        Vec3f rotated_y = out.pos;


#define MOUSE_SENSITIVITY 0.1f
        // @TODO: aparentemente quando a câmera fica alinhada com o eixo vertical
        //        isso causa um "lookAt flip". pesquisar de quais outras formas
        //        costumam contornar essa situação além de simplesmente ignorar
        //        a rotação caso a câmera esteja muito perto do eixo.
        float zenith = fabs(RAD2DEG(Vec3f_angle_between(out.pos, out.up)));
        if ( 1.0f < zenith && zenith < 180.0f ) {
            float theta = -MOUSE_SENSITIVITY * atan2f( delta.y, cam_dist );
            rotated_x = Vec3f_rotate_around_axis(rotated_x, cam_basis.right, theta);
            rotated_y = rotated_x;
        }
        float phi = -MOUSE_SENSITIVITY * atan2f( delta.x, cam_dist );
        rotated_y = Vec3f_rotate_around_axis(rotated_y, cam_basis.up , phi);
        out.pos = rotated_y;

    }

    return out;
}

void handle_keypress(GLutil* glutil, XEvent event)
{
    float angle = 0.01f;
    float delta_z = 0.5f;
    Vec3f campos = glutil->camera.pos;
    Vec3f camtrg = glutil->camera.target;
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

    glutil->camera.pos = campos;
    glutil->camera.target = camtrg;
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
        Camera newcam = handle_mouse(&glutil);
        glutil.camera.pos = newcam.pos;


        uint64_t dt = begin_frame(&glutil);
        clear_bg(BLACK);
        for (int i = 0; i < CUBES_COUNT; ++i) render_model(&glutil, &CUBES[i]);
        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}
