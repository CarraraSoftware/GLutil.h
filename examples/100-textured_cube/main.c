#include <stdio.h>


#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"


const char* MODEL_FILE_PATH       = "./examples/100-textured_cube/assets/cube_tex.obj";
const char* TEXTURE_FILE_PATH     = "./examples/100-textured_cube/assets/texture.jpg";

int main(void) {
    GLutil glutil = {0};
    init(&glutil, 800, 800);
    // init(&glutil, 2560, 1440);
    set_target_fps(&glutil, 60.0f);


    ObjData obj = load_obj_file(MODEL_FILE_PATH); // loads model with default shaders and no texture
    Model cube = model_from_obj(&obj);
    cube.texture_id = load_texture(TEXTURE_FILE_PATH);

    glutil.camera->pos.z = 30.0f;


    for (size_t i = 0; i < obj.vertices.len; i++) {
        printf("[%lu] -  %f %f %f\n", i, obj.vertices.items[i].pos.x, obj.vertices.items[i].pos.y, obj.vertices.items[i].pos.z);
    }

    printf("END OF VERTS (%lu)\n", obj.vertices.len);

    for (size_t i = 0; i < obj.faces.len; i += 3) {
        printf("[%lu] -  %u %u %u\n", i, obj.faces.items[i], obj.faces.items[i+1], obj.faces.items[i+2]);
    }
    printf("END OF FACES (%lu)\n", obj.faces.len);



    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (;;) {

        while (XPending(glutil.display)) {
            XEvent event;
            XNextEvent(glutil.display, &event);
            if (event.type != KeyPress) continue;
            if (event.xkey.keycode == 'A') default_shaders(&obj); // hot reload shaders quando pressionar barra de espaço
        }

        uint64_t dt = begin_frame(&glutil);
        clear_bg(BLACK);
        render_model(&glutil, &cube);
        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}



