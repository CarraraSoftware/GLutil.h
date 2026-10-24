#include <stdio.h>


#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"





const char* MODEL_FILE_PATH       = "./examples/999-textured_cube/assets/cube_tex.obj";
const char* TEXTURE_FILE_PATH     = "./examples/999-textured_cube/assets/texture.jpg";
/*
const char* VERT_SHADER_FILE_PATH = "./examples/999-textured_cube/shaders/vs.glsl";
const char* FRAG_SHADER_FILE_PATH = "./examples/999-textured_cube/shaders/fs.glsl";
*/




/*
void do_shaders()
{
    stop_using_shaders();
    load_shader(VERT_SHADER_FILE_PATH, GLUTIL_VERT_SHADER);
    load_shader(FRAG_SHADER_FILE_PATH, GLUTIL_FRAG_SHADER);
    use_shaders();
}
*/

int main(void) {
    GLutil glutil = {0};
    init(&glutil, 800, 800);
    // init(&glutil, 2560, 1440);
    set_target_fps(&glutil, 60.0f);


    const char* file_path = MODEL_FILE_PATH;
    ObjModel obj = load_obj_file(file_path); // loads model with default shaders and no texture


    for (size_t i = 0; i < obj.vertices.len; i++) {
        printf("[%lu] -  %f %f %f\n", i, obj.vertices.items[i].pos.x, obj.vertices.items[i].pos.y, obj.vertices.items[i].pos.z);
    }

    printf("END OF VERTS (%lu)\n", obj.vertices.len);

    for (size_t i = 0; i < obj.faces.len; i += 3) {
        printf("[%lu] -  %u %u %u\n", i, obj.faces.items[i], obj.faces.items[i+1], obj.faces.items[i+2]);
    }
    printf("END OF FACES (%lu)\n", obj.faces.len);



    if (1) {  // load texture
        const char* filename = TEXTURE_FILE_PATH;
        int img_width, img_height, n;
        unsigned char *data = stbi_load(filename, &img_width, &img_height, &n, 0);
        glGenTextures(1, &obj.texture_id);
        glBindTexture(GL_TEXTURE_2D, obj.texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        // glActiveTexture(GL_TEXTURE0);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB,
            img_width, img_height,
            0, GL_RGB, GL_UNSIGNED_BYTE,
            data 
        ); 
        glGenerateMipmap(GL_TEXTURE_2D);

        // glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
        // glBindTexture(GL_TEXTURE_2D, texture);


        stbi_image_free(data);
    }

    load_obj_to_GPU(&obj);


    glEnable(GL_DEPTH_TEST);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (;;) {

        while (XPending(glutil.display)) {
            XEvent event;
            XNextEvent(glutil.display, &event);

            if (event.type != KeyPress) {
                continue;
            }

            XKeyEvent key_event = event.xkey;


            if (key_event.keycode == 'A') {
                default_shaders(&obj);
            }

        }


        uint64_t dt = begin_frame(&glutil);
        float t = ( (float)glutil.cur_t_nanos ) / (1000.0f*1000.0f*1000.0f);

        clear_bg(BLACK);

        int uniform_loc = glGetUniformLocation(obj.shader_program, "t");
        glUniform1f(uniform_loc, t);

        render_obj(&obj);



        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}



