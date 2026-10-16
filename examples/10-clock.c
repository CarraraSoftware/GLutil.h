#define GLUTIL_IMPLEMENTATION
#include "../glutil.h"

#define WINDOW_WIDTH  1500
#define WINDOW_HEIGHT 1200


int main(void) {
    GLutil glutil = {0};
    init(&glutil, WINDOW_WIDTH, WINDOW_HEIGHT);


    float theta = 0;
    for (;;) {
        uint64_t dt = begin_frame(&glutil);


        clear_bg(BLACK);


        rect( 
            (Vec2f) { .x = WINDOW_WIDTH/2.0f - 250.0f, .y = WINDOW_HEIGHT / 2.0f - 250.0f },
            500, 500, 
            (Color) { 255, 255, 255, 255 } 
        );
        Vec2f cen = (Vec2f) {
            .x = WINDOW_WIDTH  / 2.0f,
            .y = WINDOW_HEIGHT / 2.0f,
        };


        Vec2f vec = (Vec2f) {
            .x = 250,
            .y = 250,
        };

        Color clr = (Color) {
            .red   = 200,
            .green = 80,
            .blue  =  50,
            .alpha = 255,
        };
        GLutil_line(cen, GLutil_Vec2f_add(cen, vec), clr, 20.0f);

        const float ang_vel = 0.000000008;
        theta += ang_vel * dt;
        float sine   = sinf(theta);
        float cosine = cosf(theta);
        
        Vec2f vec_rot = GLutil_Vec2f_rotccw(vec, theta);
        clr.red = 80;
        clr.green = 200;
        GLutil_line(cen, GLutil_Vec2f_add(cen, vec_rot), clr, 20.0f);

        vec_rot = GLutil_Vec2f_rotcw(vec, theta);;
        clr.green = 80;
        clr.blue  = 200;
        GLutil_line(cen, GLutil_Vec2f_add(cen, vec_rot), clr, 20.0f);

        end_frame(&glutil);
    }
}
