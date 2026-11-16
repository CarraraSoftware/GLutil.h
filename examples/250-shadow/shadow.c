#include <stdio.h>

#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"

#define DIFFUSE_TEXTURE_ID    GL_TEXTURE0
#define DIFFUSE_TEXTURE_INDEX 0

#define SHADOWMAP_TEXTURE_ID    GL_TEXTURE1
#define SHADOWMAP_TEXTURE_INDEX 1


#define WINWIDTH  800
#define WINHEIGHT 800

void print_framebuffer_status(GLenum status) 
{
    switch (status) {
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE\n");
    }; break;
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME\n");
    }; break;
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL\n");
    }; break;
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE\n");
    }; break;
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER\n");
    }; break;
    case GL_FRAMEBUFFER_COMPLETE: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_COMPLETE\n");
    }; break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
    }; break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
    }; break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n");
    }; break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n");
    }; break;
    case GL_FRAMEBUFFER_UNSUPPORTED: {
        printf("FRAMEBUFFER_STATUS = GL_FRAMEBUFFER_UNSUPPORTED\n");
    }; break;
    }
}

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

    GLutil_ShaderProgram render_shader = default_shaders();
    GLutil_ShaderProgram shadow_shader = shader_new("./shaders/shadow_vert.glsl", "./shaders/shadow_frag.glsl");

    /********** FLOOR  ****************/
    ObjData ground_obj = load_obj_file( ASSETS_PATH "/floortile.obj"); 
    ground_obj.shader_program = render_shader;
    Model   ground     = model_from_obj(&ground_obj);
    ground.texture_id  = load_texture( ASSETS_PATH "/floortile.png");
    ground.scale       =  2.0f;
    ground.world_pos.y = -1.0f;
    /*******************************************************/


    /********** CUBE ****************/
    ObjData cube_obj = load_obj_file( ASSETS_PATH "/pinkube.obj");
    cube_obj.shader_program = render_shader;
    Model   cube = model_from_obj(&cube_obj);
    cube.texture_id = load_texture(ASSETS_PATH"/pinkube.jpg");
    cube.world_pos.z =  0.0f;
    cube.world_pos.y =  0.5f;
    cube.scale       =  0.25f;
    /*******************************************************/


    /********** LIGHT BALL **********/
    ObjData ball_obj = load_obj_file(ASSETS_PATH"/ball.obj");
    ball_obj.shader_program = render_shader;
    Model   ball     = model_from_obj(&ball_obj);
    ball.world_pos   = (Vec3f) {
        .x =  1.0f,
        .y =  1.5f,
        .z =  1.0f,
    };
    ball.scale = 0.05f;
    int light = add_light(&g, (PointLight) {
        .intensity =  10000000.0f,  
        .shine     =  2.0f,  
        .pos       =  ball.world_pos,
        .clr       =  (GLColor) { 1.0f, 1.0f, 1.0f },  
    });
    /*******************************************************/




    /*************** SHADOW MAP ***************/
#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT 1024
    GLuint shadow_map;    // texture que vai ter as informações do depth buffer
    glGenTextures( 1, &shadow_map ); CHECK_GL_ERROR();
    glBindTexture( GL_TEXTURE_2D, shadow_map ); CHECK_GL_ERROR();
    glTexImage2D(
        GL_TEXTURE_2D, 0,               
        GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, 
        NULL 
    ); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_BORDER); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_BORDER); CHECK_GL_ERROR();
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    GLuint shadow_buffer; // framebuffer
    glGenFramebuffers(1, &shadow_buffer);
    glBindFramebuffer( GL_FRAMEBUFFER, shadow_buffer );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
        GL_TEXTURE_2D,  shadow_map, 0
    );
    glDrawBuffer( GL_NONE );
    glReadBuffer( GL_NONE );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    /*******************************************************/

    shader_use(render_shader);
    shader_add_int(render_shader, "shadow_map", SHADOWMAP_TEXTURE_INDEX);

    float light_x = get_light(&g, light)->pos.x;
    float light_z = get_light(&g, light)->pos.z;
    for (;;) {
        begin_frame(&g);
            float t = GLutil_get_time_secs(&g);
            update_camera(&g);

            Camera cam = g.camera;

            float delta_angle = 0.01;
            Vec3f axis = (Vec3f) { 0.0f, 1.0f, 0.0f };
            ball.world_pos = Vec3f_rotate( ball.world_pos, delta_angle, axis);

            PointLight* l = get_light(&g, light);
            l->pos = ball.world_pos;
            Vec3f light_target = (Vec3f) { 0.0f, 0.0f, 0.0f };

            Vec3f dir = Vec3f_sub( l->pos, light_target );
            float angle = M_PI / 2.0f;
            Vec3f up  = Vec3f_normalize(Vec3f_rotate( dir, angle, (Vec3f){ 1.0f, 1.0f, 1.0f }));

            Camera light_cam = (Camera) {
                .pos    = l->pos,
                .target = light_target,
                .up     = up,
                .fovX   = 90.0f,
                .near   = 0.1f,
                .far    = 1000.0f
            };


            /************* SHADOW BUFFER RENDER **************/
            g.camera = light_cam;
            g.winwidth = SHADOW_WIDTH;
            g.winheight = SHADOW_HEIGHT;

            cube.geometry->shader_program   = shadow_shader;
            ball.geometry->shader_program   = shadow_shader;
            ground.geometry->shader_program = shadow_shader;

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);      CHECK_GL_ERROR();
            glBindFramebuffer( GL_FRAMEBUFFER, shadow_buffer ); CHECK_GL_ERROR();
                glClear( GL_DEPTH_BUFFER_BIT ); CHECK_GL_ERROR();
                glEnable( GL_DEPTH_TEST );
                render_model(&g, &ground);
                render_model(&g, &cube);
                render_model(&g, &ball);
            glBindFramebuffer(GL_FRAMEBUFFER, 0); CHECK_GL_ERROR();
            /******************************************************************/


            /*************** CAMERA RENDER ********************/
            glViewport(0, 0, WINWIDTH, WINHEIGHT);      CHECK_GL_ERROR();
            clear_bg(BLACK);

            g.camera    = cam;
            g.winwidth  = WINWIDTH;
            g.winheight = WINHEIGHT;

            cube.geometry->shader_program   = render_shader;
            ball.geometry->shader_program   = render_shader;
            ground.geometry->shader_program = render_shader;

            Mat4x4f light_mat = Mat4x4f_mul(
                Mat4x4f_transpose(view_matrix(light_cam)),
                Mat4x4f_transpose(projection_matrix(light_cam, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT))
            );
            shader_use(render_shader);
            shader_add_mat4f(render_shader, "light_mat", light_mat);

            glActiveTexture( SHADOWMAP_TEXTURE_ID );
            glBindTexture( GL_TEXTURE_2D, shadow_map );

            render_model(&g, &cube);
            render_model(&g, &ball);
            render_model(&g, &ground);
            /******************************************************************/

        end_frame(&g);
    }


    printf("hello world\n");
    return 0;
}
