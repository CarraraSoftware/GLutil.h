#define GLUTIL_IMPLEMENTATION
#include "../glutil.h"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 900


int main(void) {
    GLutil glutil = {0};
    GLutil_init(&glutil, WINDOW_WIDTH, WINDOW_HEIGHT);

    Vec2f center = (Vec2f) { 
        .x = (WINDOW_WIDTH  / 2.0f) ,
        .y = (WINDOW_HEIGHT / 2.0f),
    };
    int radius = 100;
    int vx = 15;
    int vy = 15;


    XEvent event;
    for (;;) {
        // XNextEvent(glutil.display, &event); @TODO: trabalhar com eventos e tals

        int new_x = center.x + vx;
        if (new_x + radius < WINDOW_WIDTH && new_x - radius >= 0) {
            center.x += vx;
        } else {
            if (new_x + radius >= WINDOW_WIDTH) {
                center.x = WINDOW_WIDTH - radius - 1;
            } else if (new_x - radius < 0) {
                center.x = radius;
            }
            vx *= -1;
        }

        int new_y = center.y + vy;
        if (new_y + radius < WINDOW_HEIGHT && new_y - radius >= 0) {
            center.y += vy;
        } else {
            if (new_y + radius >= WINDOW_HEIGHT) {
                center.y = WINDOW_HEIGHT - radius - 1;
            } else if (new_y - radius < 0) {
                center.y = radius;
            }
            vy *= -1;
        }

        clear_bg(BLACK);
        circle( center, radius, (Color) { 255, 255, 255, 255 } );

        
        glXSwapBuffers(glutil.display, glutil.window);
    }
    
    glXDestroyContext(glutil.display, glutil.ctx);
    XCloseDisplay(glutil.display);
    
    return 0;
}
