#include <stdio.h>
#include <stdarg.h>

#define GLUTIL_IMPLEMENTATION
#include "glutil.h"

#include "include/sv.h"

#define WINWIDTH  800
#define WINHEIGHT 800

typedef float(*interpolator)(float start, float end, float t);


#define ASSETS_PATH "./examples/250-shadow/assets"

typedef struct {
    int        frame_index;
    float      scale;
    Vec3f      pos;
    Quaternion orientation;
} KeyFrame;

typedef struct {
    KeyFrame* items;
    size_t    len;
    size_t    cap;
} KeyFrames;

typedef struct {
    KeyFrames frames;
    bool      playing;
    int       curframe;
    bool      loop;
} Animation;


#define LERP(a, b, t) ((a) + (t) * ((b) - (a)))


KeyFrames parse_keyframes_from_file()
{
    KeyFrames frames = {};
    vec_init(&frames);

    StringBuilder sb = sb_from_file("cube.anim");
    StringView file = sv_from_sb(sb);

    char strbuffer[1024];
    while (file.len > 0) {
        StringView frame_index_str = sv_chop_line(&file);
        StringView scale_str       = sv_chop_line(&file);
        StringView position_str    = sv_chop_line(&file);
        StringView orientation_str = sv_chop_line(&file);

        StringView separator       = sv_chop_line(&file);
        assert(separator.len == 0);
        assert(frame_index_str.len < 1024);
        assert(scale_str.len < 1024);
        assert(position_str.len < 1024);
        assert(orientation_str.len < 1024);

        memcpy(strbuffer, frame_index_str.data, frame_index_str.len * sizeof(char));
        strbuffer[frame_index_str.len] = 0;
        float frame_index_f;
        sscanf(strbuffer, "%f", &frame_index_f);

        memcpy(strbuffer, scale_str.data, frame_index_str.len * sizeof(char));
        strbuffer[scale_str.len] = 0;
        float scale;
        sscanf(strbuffer, "%f", &scale);

        memcpy(strbuffer, position_str.data, position_str.len * sizeof(char));
        strbuffer[position_str.len] = 0;
        Vec3f position;
        sscanf(strbuffer, "%f %f %f", &position.x, &position.y, &position.z);

        memcpy(strbuffer, orientation_str.data, orientation_str.len * sizeof(char));
        strbuffer[orientation_str.len] = 0;
        Quaternion orientation;
        sscanf(strbuffer, "%f %f %f %f", &orientation.x, &orientation.y, &orientation.z, &orientation.w);

        KeyFrame kf = (KeyFrame) {
            .frame_index = (int)frame_index_f,
            .scale       = scale,
            .pos         = position,
            .orientation = orientation,
        };
        vec_append(&frames, kf);
    }

    return frames;
}

// @FROM: https://easings.net/#easeInOutElastic
float elastic(float start, float end, float t) 
{
    const float c5 = (2 * M_PI) / 4.5;
    float x = t;
    t = x == 0
      ? 0
      : x == 1
      ? 1
      : x < 0.5
      ? -pow(2, 20 * x - 10) * sin((20 * x - 11.125) * c5) / 2
      :  pow(2, -20 * x + 10) * sin((20 * x - 11.125) * c5) / 2 + 1;
    return LERP(start, end, t);
}

// @FROM: https://easings.net/#easeOutBounce
float bounce(float start, float end, float t) 
{
    const float n1 = 7.5625;
    const float d1 = 2.75;

    if (t < 1 / d1) {
        t = n1 * t * t;
    } else if (t < 2 / d1) {
        t -= 1.5 / d1;
        t = n1 * t * t + 0.75;
    } else if (t < 2.5 / d1) {
        t -= 2.25 / d1;
        t = n1 * t * t + 0.9375;
    } else {
        t -= 2.625 / d1;
        t = n1 * t * t + 0.984375;
    }
    return LERP(start, end, t);
}

float sigmoid(float start, float end, float t) {
    float denom  = 1.0f + expf(-t*10.0f + 5.0f);
    float result = (float) (1.0f / denom);
    return LERP(start, end, result);
};



bool animation_step(Animation* anim, Model* m)
{
    KeyFrames frames = anim->frames;
    int curframe     = anim->curframe;

    if ( curframe > frames.items[frames.len - 1].frame_index ) {
        anim->playing = false;
        return true;
    }

    // @NOTE: isso tá certo?
    assert(curframe >= frames.items[0].frame_index);

    KeyFrame prev_kf; prev_kf.frame_index = -1;
    KeyFrame next_kf; next_kf.frame_index = -1;
    for ( int i = 0; i < frames.len; ++i ) {
        KeyFrame keyframe = frames.items[i];
        if ( keyframe.frame_index == curframe ) {
            printf("KeyFrame Index equals CurFrame!!!!\n");
            prev_kf = keyframe;
            next_kf.frame_index = -1;
            break;
        } else if ( frames.items[i].frame_index > curframe ) {
            if ( i > 0 ) {
                prev_kf = frames.items[i-1];
            } else {
                printf("ERROR: invalid frame index: current frame = %d, keyframe index = %d\n", curframe, i); 
                exit(1);
            }
            next_kf = frames.items[i];
            break;
        } else  {
            /* NOP. still searching */
        }
    }

    if ( curframe == prev_kf.frame_index ) {
        m->world_pos   = prev_kf.pos;
        m->orientation = prev_kf.orientation;
    } else {
        float t = (float)( curframe - prev_kf.frame_index ) / (float)(next_kf.frame_index - prev_kf.frame_index );

        interpolator interp_func = sigmoid;

        float prev_scale = prev_kf.scale;
        float next_scale = next_kf.scale;
        m->scale = interp_func(prev_scale, next_scale, t);

        Vec3f prev_pos = prev_kf.pos;
        Vec3f next_pos = next_kf.pos;
        float new_x = interp_func(prev_pos.x, next_pos.x, t);
        float new_y = interp_func(prev_pos.y, next_pos.y, t);
        float new_z = interp_func(prev_pos.z, next_pos.z, t);
        m->world_pos   = (Vec3f) { .x = new_x, .y = new_y, .z = new_z };

        Quaternion prev_quat = prev_kf.orientation;
        Quaternion next_quat = next_kf.orientation;
        float new_qx = interp_func(prev_quat.x, next_quat.x, t);
        float new_qy = interp_func(prev_quat.y, next_quat.y, t);
        float new_qz = interp_func(prev_quat.z, next_quat.z, t);
        float new_qw = interp_func(prev_quat.w, next_quat.w, t);
        m->orientation = (Quaternion) { .x = new_qx, .y = new_qy, .z = new_qz, .w = new_qw }; 
    }

    anim->curframe++;
    return false;
}


int main(void)
{
    Vec3f axis = (Vec3f) { .x = 0.707f, .y = 0.707f, .z = 0.0f };
    float angle = 2 * M_PI / 5.0f;
    float step = 0;
    KeyFrames frames = parse_keyframes_from_file();

    Animation anim = (Animation) {
        .frames   = frames,
        .playing  = false,
        .curframe = 1,
        .loop     = true,
    };

    GLutil g = {};
    init(&g, WINWIDTH, WINHEIGHT);
    set_target_fps(&g, 24.0f);

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
        cube.world_pos.y = 2.0f;
        cube.texture_id  = texture_from_image(ASSETS_PATH"/pinkube.jpg");
        cube.scale       = +1.0f;
    /**************************************************************************/


    /******************* LIGHT BALL *******************************************/
    ObjData ball_obj = load_obj_file(ASSETS_PATH"/ball.obj");
    Model   ball     = model_from_obj(&ball_obj);
    ball.world_pos   = (Vec3f) { .x =  7.0f, .y =  7.5f, .z =  7.0f, };
    ball.scale = 1.0f;

    int light_id = add_light(&g, (PointLight) { .intensity =  3.0f,  .shine     =  2.0f,  .pos       =  ball.world_pos, .clr       =  (GLColor) { 1.0f, 1.0f, 1.0f },  });
    /**************************************************************************/


    Model* models[] = { &cube, &ground, &ball, };

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

            if ( g.input.f1.down ) {
                if ( anim.curframe > anim.frames.items[anim.frames.len - 1].frame_index ) {
                    // cube.world_pos = (Vec3f) { 0.0f, 0.0f, 0.0f };
                }
                anim.playing = false;
            }

            if ( g.input.f2.down ) {
                if ( anim.curframe > anim.frames.items[anim.frames.len - 1].frame_index ) {
                    anim.curframe = 1;
                }
                anim.playing = true;
            }

            if ( g.input.space_bar.down ) {
                shader_reload(shadow_shaders());
                shader_reload(render_shaders());
            }

            if ( anim.playing ) {
                bool done = animation_step(&anim, &cube);
                if ( done ) {
                    printf("---\n");
                    printf("Animation Done!!!\n");
                    printf("Animation = { \n");
                    printf("    .curframe = %d\n", anim.curframe);
                    printf("    .playing  = %s\n", anim.playing ? "true" : "false");
                    printf("}\n");
                    printf("---\n");
                    printf("Cube At: { x=%f, y=%f, z=%f }\n", cube.world_pos.x, cube.world_pos.y, cube.world_pos.z);
                    printf("---\n");
                }
                if ( done && anim.loop ) {
                    anim.playing = true;
                    anim.curframe = 1;
                }
            }

            // cube.orientation = quat_mul(cube.orientation, quat_from_angle_axis(2 * delta_angle, axis));
/******************************************************************************/


/************************** SHADOWMAP *****************************************/
            compute_shadow_maps(&g, 2, (Model*[]){ &cube, &ground });
/******************************************************************************/
            
/************************* RENDER *********************************************/
            clear_bg(BLACK);
            for ( int i = 0; i < models_count; ++i )
                render_model(&g, models[i]);
/******************************************************************************/

        end_frame(&g);
    }


    printf("hello world\n");
    return 0;
}
