#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#define GLUTIL_IMPLEMENTATION
#include "../../glutil.h"

#define CARR_VEC_IMPLEMENTATION
#include "include/vec.h"

#define CARR_SV_IMPLEMENTATION
#include "include/sv.h"



const char* MODEL_FILE_PATH       = "./examples/999-textured_cube/assets/cube_tex.obj";
const char* TEXTURE_FILE_PATH     = "./examples/999-textured_cube/assets/texture.jpg";
const char* VERT_SHADER_FILE_PATH = "./examples/999-textured_cube/shaders/vs.glsl";
const char* FRAG_SHADER_FILE_PATH = "./examples/999-textured_cube/shaders/fs.glsl";


StringView NORMAL_TAG  = (StringView) { .data = "vn", .len  = 2  };
StringView TEXTURE_TAG = (StringView) { .data = "vt", .len  = 2  };
StringView SMOOTH_TAG  = (StringView) { .data = "s",  .len  = 1  };
StringView OBJECT_TAG  = (StringView) { .data = "o",  .len  = 1  };
StringView VERTEX_TAG  = (StringView) { .data = "v",  .len  = 1  };
StringView FACE_TAG    = (StringView) { .data = "f",  .len  = 1  };


#define EVENTS_CAP 108
XEvent EVENTS[EVENTS_CAP];
int EVENTS_SIZE = 0;


typedef struct {
    Vertex* items;
    size_t len;
    size_t cap;
} Vertices;

typedef struct {
    unsigned int* items;
    size_t        len;
    size_t        cap;
} Faces;


typedef struct {
    float* items;
    size_t len;
    size_t cap;
} VecFloats;


void load_obj_file(const char* file_path, Vertices* verts, Faces* faces)
{
    StringBuilder sb = sb_from_file(file_path);
    StringView file = sv_from_sb(sb);

    VecFloats vs = {0};
    vec_init(&vs);

    VecFloats vts = {0};
    vec_init(&vts);

    Faces v_idxs = {0};
    vec_init(&v_idxs);

    Faces vt_idxs = {0};
    vec_init(&vt_idxs);


    while (file.len > 0) {
        StringView line = sv_chop_line(&file);
        if (line.len == 0 || sv_first(&line) == '#')
            continue;

        StringView tag = sv_chop_by_space(&line); 

        if (sv_is_equal(tag, NORMAL_TAG) || 
            sv_is_equal(tag, OBJECT_TAG) || sv_is_equal(tag, SMOOTH_TAG)) {
            printf("[WARNING] Vertex Data not supported: '%.*s'\n", (int)tag.len, tag.data);
        } else if ( sv_is_equal(tag, VERTEX_TAG) ) {

            char linebuf[100] = {0};
            assert(line.len < 100);
            memcpy(linebuf, line.data, sizeof(char) * line.len);

            float x, y, z;
            sscanf(linebuf, "%f %f %f", &x, &y, &z);


            vec_append(&vs, x);
            vec_append(&vs, y);
            vec_append(&vs, z);

        } else if ( sv_is_equal(tag, TEXTURE_TAG) ) {

            char linebuf[100] = {0};
            assert(line.len < 100);
            memcpy(linebuf, line.data, sizeof(char) * line.len);

            float u, v;
            sscanf(linebuf, "%f %f", &u, &v);

            vec_append(&vts, u);
            vec_append(&vts, v);

        } else if ( sv_is_equal(tag, FACE_TAG) ) {

            char linebuf[100] = {0};
            assert(line.len < 100);
            memcpy(linebuf, line.data, sizeof(char) * line.len);

            char buf1[35];
            char buf2[35];
            char buf3[35];
            
            sscanf(linebuf, "%s %s %s", buf1, buf2, buf3);

            StringView buf_sv = sv_from_cstr(buf1);
            StringView idx = sv_chop_by_delim(&buf_sv, '/');
            int idx_int = sv_parse_int(&idx);
            vec_append(&v_idxs, idx_int - 1);
            if (buf_sv.len > 0) {
                idx = sv_chop_by_delim(&buf_sv, '/');
                idx_int = sv_parse_int(&idx);
                vec_append(&vt_idxs, idx_int - 1);
                // @TODO: coloca idx_int no vec de índices para texturas
                if (buf_sv.len > 0) {
                    idx = sv_chop_by_delim(&buf_sv, '/');
                    idx_int = sv_parse_int(&idx);
                    // @TODO: criar mecanismo para lidar com normais
                    // @TODO: coloca idx_int no vec de índices para normais
                }
            }

            buf_sv = sv_from_cstr(buf2);
            idx = sv_chop_by_delim(&buf_sv, '/');
            idx_int = sv_parse_int(&idx);
            vec_append(&v_idxs, idx_int - 1);
            if (buf_sv.len > 0) {
                idx = sv_chop_by_delim(&buf_sv, '/');
                idx_int = sv_parse_int(&idx);
                vec_append(&vt_idxs, idx_int - 1);
                // @TODO: coloca idx_int no vec de índices para texturas
                if (buf_sv.len > 0) {
                    idx = sv_chop_by_delim(&buf_sv, '/');
                    idx_int = sv_parse_int(&idx);
                    // @TODO: criar mecanismo para lidar com normais
                    // @TODO: coloca idx_int no vec de índices para normais
                }
            }

            buf_sv = sv_from_cstr(buf3);
            idx = sv_chop_by_delim(&buf_sv, '/');
            idx_int = sv_parse_int(&idx);
            vec_append(&v_idxs, idx_int - 1);
            if (buf_sv.len > 0) {
                idx = sv_chop_by_delim(&buf_sv, '/');
                idx_int = sv_parse_int(&idx);
                vec_append(&vt_idxs, idx_int - 1);
                // @TODO: coloca idx_int no vec de índices para texturas
                if (buf_sv.len > 0) {
                    idx = sv_chop_by_delim(&buf_sv, '/');
                    idx_int = sv_parse_int(&idx);
                    // @TODO: criar mecanismo para lidar com normais
                    // @TODO: coloca idx_int no vec de índices para normais
                }
            }



        } else {
            printf("[WARNING] Unknown Vertex Data Tag: '%.*s'\n", (int)tag.len, tag.data);
        }

    }


    if (vt_idxs.len != 0) {
        assert(vt_idxs.len == v_idxs.len);
    }

    /* @TODO: array de índices para normais
    if (vn_idxs.len != 0) {
        assert(vn_idxs.len == v_idxs.len);
    }
    */

    printf("VIDXS   => { data: %p, len: %lu, cap: %lu }\n", v_idxs.items, v_idxs.len, v_idxs.cap);
    printf("VT_IDXS => { data: %p, len: %lu, cap: %lu }\n", vt_idxs.items, vt_idxs.len, vt_idxs.cap);

    printf(" ARRAY DE COORDENADAS \n");
    for (int i = 0; i < vs.len; ++i) {
        printf("[%d] %f\n", i, vs.items[i]);
    }



    for (int i = 0; i < v_idxs.len; ++i) {
        int vidx = v_idxs.items[i] * 3;
        
        printf("----\n");
        printf("VERTEX IDX = %d\n", vidx);

        float x = vs.items[vidx + 0];
        float y = vs.items[vidx + 1];
        float z = vs.items[vidx + 2];

        printf(" x = %f, y = %f, z = %f \n", x, y, z);

        float u = 0.0f;
        float v = 0.0f;

        if (vt_idxs.len > 0) {
            int vtidx = vt_idxs.items[i] * 2;
            u = vts.items[vtidx + 0];
            v = vts.items[vtidx + 1];
        }

        Vertex vertex = {
            .pos     = { .x = x,    .y = y,    .z = z,   },
            .color   = { .r = 1.0f, .g = 1.0f, .b = 1.0f },
            .tex_pos = { .x = u,    .y = v               },
        };

        vec_append(verts, vertex);
        vec_append(faces, verts->len - 1);

        printf("  FINAL FACE IDX = %lu\n", verts->len - 1);
        printf("-----\n");
        // __asm__("int3");

    }

    vec_free(&vs);
    vec_free(&vts);
    vec_free(&v_idxs);
    vec_free(&vt_idxs);
    sb_free(&sb);

    return;
}

void do_shaders()
{
    stop_using_shaders();
    load_shader(VERT_SHADER_FILE_PATH, GLUTIL_VERT_SHADER);
    load_shader(FRAG_SHADER_FILE_PATH, GLUTIL_FRAG_SHADER);
    use_shaders();
}

int main(void) {
    GLutil glutil = {0};
    init(&glutil, 800, 800);
    // init(&glutil, 2560, 1440);
    set_target_fps(&glutil, 60.0f);

    do_shaders();

    Vertices verts = {0};
    vec_init(&verts);

    Faces faces = {0};
    vec_init(&faces);

    const char* file_path = MODEL_FILE_PATH;
    load_obj_file(file_path, &verts, &faces);

    for (int i = 0; i < verts.len; i++) {
        printf("[%d] -  %f %f %f\n", i, verts.items[i].pos.x, verts.items[i].pos.y, verts.items[i].pos.z);
    }

    printf("END OF VERTS (%lu)\n", verts.len);

    for (int i = 0; i < faces.len; i += 3) {
        printf("[%d] -  %u %u %u\n", i, faces.items[i], faces.items[i+1], faces.items[i+2]);
    }
    printf("END OF FACES (%lu)\n", faces.len);


    const char* filename = TEXTURE_FILE_PATH;
    int img_width, img_height, n;
    unsigned char *data = stbi_load(filename, &img_width, &img_height, &n, 0);


    printf("n = %d\n", n);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
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

    glUniform1i(glGetUniformLocation(current_shader_program, "the_texture"), 0);

    load_vertices_and_faces(verts.items, verts.len, faces.items, faces.len);

    stbi_image_free(data);

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
                do_shaders();
            }

        }


        uint64_t dt = begin_frame(&glutil);
        float t = ( (float)glutil.cur_t_nanos ) / (1000.0f*1000.0f*1000.0f);

        clear_bg(BLACK);

        int uniform_loc = glGetUniformLocation(current_shader_program, "t");
        glUniform1f(uniform_loc, t);



        glBindTexture(GL_TEXTURE_2D, texture);
        draw();

        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}



