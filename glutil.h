/* 
* Most of the OpenGl initialization code was taken from:
* https://github.com/ColleagueRiley/OpenGL-Context-Creation
* made by the author of
* https://github.com/ColleagueRiley/RGFW 
* 
* with license:
*
*	RGFW 2.0.0-dev

* Copyright (C) 2022-26 Riley Mabb (@ColleagueRiley)
*
* libpng license
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.

* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*/

#ifndef GLUTIL_H_
#define GLUTIL_H_ 

#include <GL/gl.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#define CARR_VEC_FORCE_PREFIX
#define CARR_VEC_IMPLEMENTATION
#include "include/vec.h"

#define CARR_SV_FORCE_PREFIX
#define CARR_SV_IMPLEMENTATION
#include "include/sv.h"



#ifndef GLUTIL_FORCE_PREFIX

#define MAP    GLUTIL_MAP
#define RAND   GLUTIL_RAND
#define COLOR  GLUTIL_COLOR
#define BLACK  GLUTIL_BLACK

#define Color   GLutil_Color

#define Vec2f   GLutil_Vec2f
#define Vec3f   GLutil_Vec3f
#define Vec4f   GLutil_Vec4f

#define Mat2x2f  GLutil_Mat2x2f
#define Mat2f    GLutil_Mat2f
#define Mat3x3f  GLutil_Mat3x3f
#define Mat3f    GLutil_Mat3f
#define Mat4x4f  GLutil_Mat4x4f
#define Mat4f    GLutil_Mat4f


#define Vertex   GLutil_Vertex
#define Vertices GLutil_Vertices
#define Faces    GLutil_Faces
#define ObjData  GLutil_ObjData
#define Model    GLutil_Model

#define init    GLutil_init
#define destroy GLutil_destroy

#define circle   GLutil_circle
#define line     GLutil_line
#define rect     GLutil_rect
#define ortho2D  GLutil_ortho2D
#define clear_bg GLutil_clear_bg

#define wireframe_mode GLutil_wireframe_mode

#define Vec2f_add      GLutil_Vec2f_add
#define Vec2f_sub      GLutil_Vec2f_sub
#define Vec2f_sqlength GLutil_Vec2f_sqlength
#define Vec2f_length   GLutil_Vec2f_length
#define Vec2f_rotccw   GLutil_Vec2f_rotccw
#define Vec2f_rotcw    GLutil_Vec2f_rotcw
#define Vec2f_dot GLutil_Vec2f_dot
#define Vec2f_apply_mat2x2 GLutil_Vec2f_apply_mat2x2
#define Vec2f_scale GLutil_Vec2f_scale

#define Vec3f_add GLutil_Vec3f_add
#define Vec3f_sub GLutil_Vec3f_sub
#define Vec3f_sqlength GLutil_Vec3f_sqlength
#define Vec3f_length GLutil_Vec3f_length
#define Vec3f_div GLutil_Vec3f_div
#define Vec3f_normalize GLutil_Vec3f_normalize
#define Vec3f_scale GLutil_Vec3f_scale
#define Vec3f_dot GLutil_Vec3f_dot
#define Vec3f_cross GLutil_Vec3f_cross

#define Mat2x2f_mul GLutil_Mat2x2f_mul
#define Mat3x3f_mul GLutil_Mat3x3f_mul
#define Mat4x4f_mul GLutil_Mat4x4f_mul

#define PRINT_ERROR     GLUTIL_PRINT_ERROR
#define CHECK_GL_ERROR  GLUTIL_CHECK_GL_ERROR
#define check_gl_error_ GLutil_check_gl_error_

#define get_time_nanos    GLutil_get_time_nanos
#define begin_frame       GLutil_begin_frame
#define frame_delta_nanos GLutil_frame_delta_nanos
#define end_frame         GLutil_end_frame
#define present           GLutil_present
#define set_target_fps    GLutil_set_target_fps
#define next_event        GLutil_next_event
#define t_secs            GLutil_t_secs

#define load_shader        GLutil_load_shader
#define default_shaders    GLutil_default_shaders

#define render_obj       GLutil_render_obj
#define load_obj_file    GLutil_load_obj_file
#define load_obj_to_GPU  GLutil_load_obj_to_GPU
#define load_texture     GLutil_load_texture
#define model_from_obj   GLutil_model_from_obj
#define render_model     GLutil_render_model

#endif // GLUTIL_INCLUDE_PREFIX

#define GLUTIL_MAP(x, src_min, src_max, dst_min, dst_max) ( (dst_min) + ( ( ( (x) - (src_min) ) * ( (dst_max) - (dst_min) ) ) / ( (src_max) - (src_min) ) ))
#define GLUTIL_RAND(min, max) MAP((float)rand(), 0.0f, (float)RAND_MAX, (float)(min), (float)(max))

#define GLUTIL_COLOR(r, g, b) (Color){ .red = (r), .green = (g), .blue = (b), .alpha = 255 }
#define GLUTIL_BLACK COLOR(0, 0, 0)

#define GLUTIL_PRINT_ERROR(msg) fprintf(stderr, "%d:%s:%s ERROR: %s", __LINE__, __FILE__, __FUNCTION__, msg)
#define GLUTIL_CHECK_GL_ERROR() GLutil_check_gl_error_(__LINE__, __FILE__, __FUNCTION__)

#define ALL_INPUT_MASKS ( \
        KeyPressMask       | KeyReleaseMask         | ButtonPressMask          \
      | ButtonReleaseMask  | EnterWindowMask        | LeaveWindowMask          \
      | PointerMotionMask  | PointerMotionHintMask  | Button1MotionMask        \
      | Button2MotionMask  | Button3MotionMask      | Button4MotionMask        \
      | Button5MotionMask  | ButtonMotionMask       | KeymapStateMask          \
      | ExposureMask       | VisibilityChangeMask   | StructureNotifyMask      \
      | ResizeRedirectMask | SubstructureNotifyMask | SubstructureRedirectMask \
      | FocusChangeMask    | PropertyChangeMask     | ColormapChangeMask       \
      | OwnerGrabButtonMask )


typedef struct GLutil_Camera GLutil_Camera;

typedef struct GLutil {
    Window       window;
    Display*     display;
    GLXContext   ctx;
    XVisualInfo* vi;
    GLXFBConfig  fbc;

    int winwidth;
    int winheight;


    uint64_t     prev_t_nanos;    // timestamp em nanosegundos do último frame
    uint64_t     cur_t_nanos;     // timestamp em nanosegundos do frame atual
    uint64_t     dt_nanos;        // delta time em nanosegundos entre os dois últimos frames
    uint64_t     target_sleep_us; // quantidade de microsegundos de delay para atingir o FPS alvo

    GLuint       current_vao ;
    int          current_ebo_count;

    GLutil_Camera* camera; // @TODO: não consegui organizar as informações de tipos
                           //        de forma a ter acesso à Camera antes da declaração
                           //        da struct GLutil, então deixei como pointer mesmo.
                           //        mas acredito que ter o a struct por valor seria melhor.

} GLutil;


typedef struct {
    unsigned char red;  
    unsigned char green;
    unsigned char blue; 
    unsigned char alpha;
} GLutil_Color;


typedef struct  {
    float x, y;
} GLutil_Vec2f;


typedef struct {
    float x, y, z;
} GLutil_Vec3f;

typedef struct {
    float x, y, z, w;
} GLutil_Vec4f;

typedef union {
    float data[4];

    Vec2f rows[2];

    struct {
        // _[row][col]
        float _11; float _12;
        float _21; float _22;
    };

} GLutil_Mat2x2f;

typedef GLutil_Mat2x2f GLutil_Mat2f;

typedef struct {
    union {
        float data[9];

        Vec3f rows[3];

        struct {
            float _11; float _12; float _13;
            float _21; float _22; float _23;
            float _31; float _32; float _33;
        };
    };


} GLutil_Mat3x3f;

typedef GLutil_Mat3x3f GLutil_Mat3f;

typedef struct {
    union {
        float data[16];

        Vec4f rows[4];

        struct {
            float _11; float _12; float _13; float _14;
            float _21; float _22; float _23; float _24;
            float _31; float _32; float _33; float _34;
            float _41; float _42; float _43; float _44;
        };  
    };
} GLutil_Mat4x4f;

typedef GLutil_Mat4x4f GLutil_Mat4f;

 typedef enum {
    GLUTIL_VERT_SHADER = GL_VERTEX_SHADER,
    GLUTIL_FRAG_SHADER = GL_FRAGMENT_SHADER,
} GLutil_ShaderType;


#define GLutil_Vertex_XYZ_size 3
typedef struct {
    Vec3f pos;

    struct {
        float r;
        float g;
        float b;
    } color;


    Vec2f tex_pos;

} GLutil_Vertex;


typedef struct {
    GLutil_Vertex* items;
    size_t         len;
    size_t         cap;
} GLutil_Vertices;

typedef struct {
    unsigned int* items;
    size_t        len;
    size_t        cap;
} GLutil_Faces;

struct GLutil_Camera {
    GLutil_Vec3f pos;
    GLutil_Vec3f target;
    GLutil_Vec3f up;

    float fovX;
    float near;
    float far;

};

typedef struct {
    GLutil_Vertices vertices;  // vertex data parsed from .obj file
    GLutil_Faces    faces;     // face   data parsed from .obj file

    GLuint vbo_id, ebo_id, vao_id;  // ids (or names?) returned by openGL when vertex data is loaded into the GPU


    // @NOTE: 
    // talvez esse campo deva ser interpretado como o shader padrão associado
    // a essa model, mas através de um Vec<Shader> em algum lugar podemos
    // permitir adicionar mais shaders dinamicamente
    GLuint shader_program;          // id of compiled and linked shader program to use
                                    // a value of -1 (4294967295), means there's no shader associated with this model 
} GLutil_ObjData;

typedef struct {
    float    scale;
    Vec3f    world_pos;

    GLutil_ObjData* geometry;
    GLuint           texture_id;  // id to the texture_id returned by OpenGL when the texture data is loaded into the GPU
                                  // a value of -1 (4294967295), means there's no texture associated with this model 
} GLutil_Model;


#define GLUTIL_DEFAULT_CAM_FOV  60.0f
#define GLUTIL_DEFAULT_CAM_NEAR 0.1f
#define GLUTIL_DEFAULT_CAM_FAR  100.0f

GLutil_Camera GLUTIL_DEFAULT_CAMERA = {
    .pos =    { .x = 0.0f, .y = 0.0f, .z = 3.0f },
    .target = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
    .up =     { .x = 0.0f, .y = 1.0f, .z = 0.0f },

    .fovX = GLUTIL_DEFAULT_CAM_FOV,
    .near = GLUTIL_DEFAULT_CAM_NEAR,
    .far  = GLUTIL_DEFAULT_CAM_FAR,
};



void GLutil_init(GLutil* glutil, unsigned int w, unsigned int h);            // inicializa Display, Window e o OpenGLContext. e mais alguns estados internos
void GLutil_destroy(GLutil* glutil);                                         // desaloca e fecha todos os recursos alocados por GLutil.h


// api legado do OpenGL
void GLutil_circle(GLutil_Vec2f center, int radius, GLutil_Color color);     // desenha um círculo através de GL_TRIANGLE_FAN, o nível de precisão do círculo pode ser alterado pelo macro GLUTIL_CIRCLE_PRECISION
void GLutil_line(GLutil_Vec2f a, GLutil_Vec2f b, GLutil_Color clr, float w);
void GLutil_rect(GLutil_Vec2f topleft, float w, float h, GLutil_Color clr);  
void GLutil_ortho2D(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom);

GLutil_Vec2f GLutil_Vec2f_add(GLutil_Vec2f a, GLutil_Vec2f b);
GLutil_Vec2f GLutil_Vec2f_sub(GLutil_Vec2f a, GLutil_Vec2f b);
float GLutil_Vec2f_sqlength(GLutil_Vec2f v);
float GLutil_Vec2f_length(GLutil_Vec2f v);
GLutil_Vec2f GLutil_Vec2f_rotccw(GLutil_Vec2f v, float theta);
GLutil_Vec2f GLutil_Vec2f_rotcw(GLutil_Vec2f v, float theta);
float GLutil_Vec2f_dot(GLutil_Vec2f a, GLutil_Vec2f b);
GLutil_Vec2f GLutil_Vec2f_apply_mat2x2(GLutil_Vec2f v, Mat2x2f A);
GLutil_Vec2f GLutil_Vec2f_scale(GLutil_Vec2f v, float sx, float sy);


GLutil_Vec3f GLutil_Vec3f_add(GLutil_Vec3f a, GLutil_Vec3f b);
GLutil_Vec3f GLutil_Vec3f_sub(GLutil_Vec3f a, GLutil_Vec3f b);
float GLutil_Vec3f_sqlength(GLutil_Vec3f v);
float GLutil_Vec3f_length(GLutil_Vec3f v);
GLutil_Vec3f GLutil_Vec3f_div(GLutil_Vec3f v, float d);
GLutil_Vec3f GLutil_Vec3f_normalize(GLutil_Vec3f v);
GLutil_Vec3f GLutil_Vec3f_scale(GLutil_Vec3f v, float s);
float GLutil_Vec3f_dot(GLutil_Vec3f a, GLutil_Vec3f b);
GLutil_Vec3f GLutil_Vec3f_cross(GLutil_Vec3f a, GLutil_Vec3f b);

GLutil_Mat2x2f GLutil_Mat2x2f_mul(GLutil_Mat2x2f A, GLutil_Mat2x2f B);
GLutil_Mat3x3f GLutil_Mat3x3f_mul(GLutil_Mat3x3f A, GLutil_Mat3x3f B);
GLutil_Mat4x4f GLutil_Mat4x4f_mul(GLutil_Mat4x4f a, GLutil_Mat4x4f b);

void GLutil_check_gl_error_(int line, const char* file, const char* func);

// FRAME LOOP
uint64_t GLutil_get_time_nanos(void);
uint64_t GLutil_frame_delta_nanos(GLutil *glutil);
uint64_t GLutil_begin_frame(GLutil *glutil);
void GLutil_clear_bg( Color color );
void GLutil_end_frame(GLutil *glutil); // faz o swap dos buffers E atualiza o estado interno do GLutil (principalmente o delta entre frames)
void GLutil_present(GLutil* glutil);  // faz o swap dos buffers do opengl
void GLutil_set_target_fps(GLutil* glutil, float fps); // usa fps para calcular o delay entre frames como 1/fps e espera o delay somente se delay > 0
bool GLutil_next_event(GLutil* glutil, XEvent* event);
float GLutil_t_secs(GLutil* glutil);

// CAMERA
GLutil_Mat3x3f GLutil_model_matrix(GLutil_Camera cam);
GLutil_Mat4x4f GLutil_view_matrix(GLutil_Camera cam);
GLutil_Mat4x4f GLutil_projection_matrix(GLutil_Camera cam, float aspectRatio); // @TODO: pensar sobre onde armazenar as informações de width e height da janela, de maneira a opcionalmente não precisar passar o argumento aspectRatio 

// SHADERS
GLuint GLutil_load_shader(const char* name, GLutil_ShaderType type);
void GLutil_default_shaders(GLutil_ObjData* obj);


// MODELS, OBJ E TEXTURES
GLutil_ObjData GLutil_load_obj_file(const char* file_path);
void GLutil_load_obj_to_GPU(GLutil_ObjData* obj);

void GLutil_render_model(GLutil* glutil, GLutil_Model* model);

GLuint GLutil_load_texture(const char* texture_name);

void GLutil_wireframe_mode();
GLutil_Model GLutil_model_from_obj(GLutil_ObjData* obj);



#endif // GLUTIL_H_



/**************************** FIM DO HEADER ***********************************/



#ifdef GLUTIL_IMPLEMENTATION


static const char* DEFAULT_VERT_SHADER_PATH = "./shaders/default_vs.glsl";
static const char* DEFAULT_FRAG_SHADER_PATH = "./shaders/default_fs.glsl";

// const char* GL_ERROR_NAMES[] = {
//     [0] = "GL_NO_ERROR ",
//     [0x0500] = "GL_INVALID_ENUM",
//     [0x0501] = "GL_INVALID_VALUE",
//     [0x0502] = "GL_INVALID_OPERATION",
//     [0x0503] = "GL_STACK_OVERFLOW",
//     [0x0504] = "GL_STACK_UNDERFLOW",
//     [0x0505] = "GL_OUT_OF_MEMORY",
// };


const char* INPUT_TYPE_NAMES[] = {
    [0]  = "[RESERVED]",
    [1]  = "[RESERVED]",
    [2]  = "KeyPress",
    [3]  = "KeyRelease",
    [4]  = "ButtonPress",
    [5]  = "ButtonRelease",
    [6]  = "MotionNotify",
    [7]  = "EnterNotify",
    [8]  = "LeaveNotify",
    [9]  = "FocusIn",
    [10] = "FocusOut",
    [11] = "KeymapNotify",
    [12] = "Expose",
    [13] = "GraphicsExpose",
    [14] = "NoExpose",
    [15] = "VisibilityNotify",
    [16] = "CreateNotify",
    [17] = "DestroyNotify",
    [18] = "UnmapNotify",
    [19] = "MapNotify",
    [20] = "MapRequest",
    [21] = "ReparentNotify",
    [22] = "ConfigureNotify",
    [23] = "ConfigureRequest",
    [24] = "GravityNotify",
    [25] = "ResizeRequest",
    [26] = "CirculateNotify",
    [27] = "CirculateRequest",
    [28] = "PropertyNotify",
    [29] = "SelectionClear",
    [30] = "SelectionRequest",
    [31] = "SelectionNotify",
    [32] = "ColormapNotify",
    [33] = "ClientMessage",
    [34] = "MappingNotify",
    [35] = "GenericEvent",
};


/*******************************************************************************
*      TABELA DE XMACRO PARA FUNÇÕES CARREGADAS DURANTE O RUNTIME              *
*                                                                              *
******************************************************************************/ 


//   function_name                return_type   ...params
#define GLUTIL_GL_FUNCTIONS_TO_LOAD                                                                                                                    \
   X( glXCreateContextAttribsARB, GLXContext,   Display*, GLXFBConfig, GLXContext, Bool, const int*                                                  ) \
   X( glGenBuffers,               void,         GLsizei n, GLuint *buffers                                                                           ) \
   X( glBindBuffer,               void,         GLenum target, GLuint  buffer                                                                        ) \
   X( glBufferData,               void,         GLenum target, GLsizeiptr size, const void *data, GLenum usage                                       ) \
   X( glCreateShader,             GLuint,       GLenum type                                                                                          ) \
   X( glShaderSource,             void,         GLuint shader,  GLsizei count, const GLchar *const*string, const GLint *length                       ) \
   X( glCompileShader,            void,         GLuint shader                                                                                        ) \
   X( glAttachShader,             void,         GLuint program, GLuint shader                                                                        ) \
   X( glDeleteShader,             void,         GLuint shader                                                                                        ) \
   X( glCreateProgram,            GLuint,       void                                                                                                 ) \
   X( glLinkProgram,              void,         GLuint program                                                                                       ) \
   X( glUseProgram,               void,         GLuint program                                                                                       ) \
   X( glDeleteProgram,            void,         GLuint program                                                                                       ) \
   X( glVertexAttribPointer,      void,         GLuint index, GLint size,     GLenum type, GLboolean normalized, GLsizei stride, const void *pointer ) \
   X( glEnableVertexAttribArray,  void,         GLuint index                                                                                         ) \
   X( glGenVertexArrays,          void,         GLsizei n,    GLuint *arrays                                                                         ) \
   X( glBindVertexArray,          void,         GLuint array                                                                                         ) \
   X( glGetProgramiv,             void,         GLuint program, GLenum pname, GLint *params                                                          ) \
   X( glGetUniformLocation,       GLint,        GLuint program, const GLchar *name                                                                   ) \
   X( glGenerateMipmap,           void,         GLenum target                                                                                        ) \
   X( glUniform1i,                void,         GLint location, GLint v0                                                                             ) \
   X( glUniform1f,                void,         GLint location, GLfloat v0                                                                           ) \
   X( glUniform3fv,               void,         GLint location, GLsizei count, const GLfloat* value                                                  ) \
   X( glUniformMatrix3fv,         void,         GLint location, GLsizei count, GLboolean transpose, const GLfloat* value                             ) \
   X( glUniformMatrix4fv,         void,         GLint location, GLsizei count, GLboolean transpose, const GLfloat* value                             )
    
   // X( glGenTextures,              void,         GLsizei n, GLuint* textures                                                                          )
      
#define GLUTIL_CONCAT_MACRO_NAMES(x, y) x ## y

// definições de tipo para todas as funções necessárias de serem carregadas dinamicamente
#define X(func_name, ret_type, ...) typedef ret_type ( * GLUTIL_CONCAT_MACRO_NAMES(func_name, Proc) ) ( __VA_ARGS__ );
GLUTIL_GL_FUNCTIONS_TO_LOAD
#undef X

// váriaveis globais pras funções que serão carregadas dinamicamente
#define X(func_name, ...) GLUTIL_CONCAT_MACRO_NAMES(func_name, Proc) func_name;
GLUTIL_GL_FUNCTIONS_TO_LOAD
#undef X


/*******************************************************************************
*  INICIALIZAÇÃO DO OPENGL                                                    *
*  (from: https://github.com/ColleagueRiley/OpenGL-Context-Creation)          *
******************************************************************************/ 


typedef uint8_t GL_profile; 
enum  { 
    GL_CORE = 0,  
    GL_COMPATIBILITY  
};
int32_t majorVersion = 0, minorVersion = 0;
Bool profile = GL_CORE;

static const int32_t STENCIL = 0, SAMPLES = 0, STEREO = GL_FALSE, AUX_BUFFERS = 0, DOUBLE_BUFFER = 1;


void GLutil_make_display(GLutil* glutil) 
{
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    glutil->display = display;
}


void GLutil_make_window(GLutil* glutil, int w, int h)
{
    XSetWindowAttributes swa;
    Colormap cmap;

    swa.colormap = cmap = XCreateColormap(glutil->display, DefaultRootWindow(glutil->display), glutil->vi->visual, AllocNone);

    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = CWColormap | CWBorderPixel | CWBackPixel | CWEventMask;
    
    swa.background_pixel = 0;

    Window window = XCreateWindow(
            glutil->display, DefaultRootWindow(glutil->display), // display, parent
            50, 50,    // x, y
            w, h, // w, h
            0,         // border-width
            glutil->vi->depth, 
            InputOutput, 
            glutil->vi->visual,
            CWColormap | CWBorderPixel | CWBackPixel | CWEventMask, 
            &swa
    );
    glutil->window = window;
}


void GLutil_make_fbc_and_vi(GLutil* glutil)
{
    static uint32_t attribs[] = {
        GLX_X_VISUAL_TYPE,      GLX_TRUE_COLOR,
        GLX_DEPTH_SIZE,         24,                            
        GLX_X_RENDERABLE,       1,
        GLX_RED_SIZE,           8,
        GLX_GREEN_SIZE,         8,
        GLX_BLUE_SIZE,          8,
        GLX_ALPHA_SIZE,         8,
        GLX_RENDER_TYPE,        GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE,      GLX_WINDOW_BIT,

        GLX_DOUBLEBUFFER,       1,
        GLX_STENCIL_SIZE,       STENCIL,
        GLX_STEREO,             STEREO,
        GLX_AUX_BUFFERS,        AUX_BUFFERS,

        0,                      0, 
        0,                      0, 
        0
    };
    
    int32_t fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(glutil->display, DefaultScreen(glutil->display), (int32_t*) attribs, &fbcount);

    int32_t best_fbc = -1;

    if (fbcount == 0) {
        printf("Failed to find any valid GLX visual configs\n");
        exit(2);
    }
  
    uint32_t i;
    for (i = 0; i < (uint32_t)fbcount; i++) {
        int32_t samp_buf, samples;
        glXGetFBConfigAttrib(glutil->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
        glXGetFBConfigAttrib(glutil->display, fbc[i], GLX_SAMPLES, &samples);
        
        if ((best_fbc < 0 || samp_buf) && (samples == SAMPLES || best_fbc == -1)) {
            best_fbc = i;
        }
    }

    if (best_fbc == -1) {
        printf("Failed to get a valid GLX visual\n");
        exit(3);
    }
    glutil->fbc = fbc[best_fbc];
    XFree(fbc);
    glutil->vi = glXGetVisualFromFBConfig(glutil->display, glutil->fbc);
}


void GLutil_make_context(GLutil* glutil)
{
    XSelectInput(glutil->display, glutil->window, ALL_INPUT_MASKS);
    int32_t context_attribs[7] = { 0, 0, 0, 0, 0, 0, 0 };
    context_attribs[0] = GLX_CONTEXT_PROFILE_MASK_ARB;
    if (profile == GL_CORE) 
        context_attribs[1] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
    else 
        context_attribs[1] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
    
    if (majorVersion || minorVersion) {
        context_attribs[2] = GLX_CONTEXT_MAJOR_VERSION_ARB;
        context_attribs[3] = majorVersion;
        context_attribs[4] = GLX_CONTEXT_MINOR_VERSION_ARB;
        context_attribs[5] = minorVersion;
    }
    

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*) "glXCreateContextAttribsARB");
    GLXContext ctx = glXCreateContextAttribsARB(glutil->display, glutil->fbc, NULL, True, context_attribs);
    glutil->ctx = ctx;
    glXMakeCurrent(glutil->display, glutil->window, glutil->ctx);
    XMapWindow(glutil->display, glutil->window);
}

void GLutil_init(GLutil* glutil, unsigned int w, unsigned int h)
{

#define X(func_name, ...) func_name = ( GLUTIL_CONCAT_MACRO_NAMES(func_name, Proc) ) glXGetProcAddressARB( (GLubyte*) #func_name );
    GLUTIL_GL_FUNCTIONS_TO_LOAD
#undef X

    glutil->winwidth = w;
    glutil->winheight = h;

    GLutil_make_display(glutil);
    GLutil_make_fbc_and_vi(glutil);
    GLutil_make_window(glutil, w, h);
    GLutil_make_context(glutil);

    // permite usar a api legado do OpenGL, mas não afeta o uso de shaders
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    GLutil_ortho2D(0, w, 0, h); // coloca origem no topo esquerda
    glEnable(GL_DEPTH_TEST);

    glutil->camera = &GLUTIL_DEFAULT_CAMERA;

    glutil->cur_t_nanos  = 0;
    glutil->prev_t_nanos = 0;
    glutil->dt_nanos     = 0;
    glutil->target_sleep_us = 16000; // 16 ms => 60 FPS

}

void GLutil_destroy(GLutil* glutil)
{
    glXDestroyContext(glutil->display, glutil->ctx);
    XCloseDisplay(glutil->display);
}



void GLutil_check_gl_error_(int line, const char* file, const char* func)
{
    GLenum err = glGetError();
    switch (err) {
		case GL_NO_ERROR: {
			// printf("%s:%d:%s GL_NO_ERROR \n", file, line, func);
		} break;
		case GL_INVALID_ENUM: {
			printf("%s:%d:%s GL_INVALID_ENUM \n", file, line, func);
            exit(1);
		} break;
		case GL_INVALID_VALUE: {
			printf("%s:%d:%s GL_INVALID_VALUE \n", file, line, func);
            exit(2);
		} break;
		case GL_INVALID_OPERATION: {
			printf("%s:%d:%s GL_INVALID_OPERATION \n", file, line, func);
            exit(3);
		} break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: {
			printf("%s:%d:%s GL_INVALID_FRAMEBUFFER_OPERATION \n", file, line, func);
            exit(4);
		} break;
		case GL_OUT_OF_MEMORY: {
			printf("%s:%d:%s GL_OUT_OF_MEMORY \n", file, line, func);
            exit(5);
		} break;
    }
}



void GLutil_ortho2D(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom)
{
    glOrtho(left, right, bottom, top, -1, 1);
}

void GLutil_wireframe_mode()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GLutil_clear_bg( Color color )
{
    glClearColor(color.red / 255.0f,  color.green / 255.0f, color.blue / 255.0f, color.alpha / 255.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void GLutil_present(GLutil* glutil)
{
    glXSwapBuffers(glutil->display, glutil->window);
}

uint64_t GLutil_get_time_nanos(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts );
    long int secs = ts.tv_sec;
    long int nanos = ts.tv_nsec;
    uint64_t t = (secs * 1000000000) + nanos;
    return t;
}

uint64_t GLutil_frame_delta_nanos(GLutil *glutil)
{
    return glutil->dt_nanos;
}

uint64_t GLutil_begin_frame(GLutil *glutil)
{
    glutil->prev_t_nanos = glutil->cur_t_nanos;
    glutil->cur_t_nanos = GLutil_get_time_nanos();
    return glutil->cur_t_nanos - glutil->prev_t_nanos;
}

void GLutil_end_frame(GLutil *glutil)
{
    GLutil_present(glutil);
    uint64_t elapsed_us = (get_time_nanos() - glutil->cur_t_nanos) / 1000;
    if (glutil->target_sleep_us > elapsed_us) 
        usleep(glutil->target_sleep_us - elapsed_us);
}

void GLutil_set_target_fps(GLutil* glutil, float fps)
{
    float target_microsecs = (1000000.0f / fps);
    glutil->target_sleep_us = (uint64_t)target_microsecs;
}


bool GLutil_next_event(GLutil* glutil, XEvent* event)
{
    return XCheckMaskEvent( glutil->display, ALL_INPUT_MASKS , event );
}

float GLutil_t_secs(GLutil* glutil)
{
    return ( (float)glutil->cur_t_nanos / 1000.0f / 1000.0f / 1000.0f );
}


/*******************************************************************************
 *                     VETORES E ALGEBRA LINEAR                                *
 *******************************************************************************/

GLutil_Vec2f GLutil_Vec2f_add(GLutil_Vec2f a, GLutil_Vec2f b) 
{
    return (GLutil_Vec2f) {
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
}

GLutil_Vec2f GLutil_Vec2f_sub(GLutil_Vec2f a, GLutil_Vec2f b)
{
    return (GLutil_Vec2f) {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

float GLutil_Vec2f_sqlength(GLutil_Vec2f v)
{
    // c² = a² + b²
    return (v.x*v.x + v.y*v.y);
}

float GLutil_Vec2f_length(GLutil_Vec2f v)
{
    // c = sqrtf(a² + b²)
    return sqrtf(GLutil_Vec2f_sqlength(v));
}

GLutil_Vec2f GLutil_Vec2f_rotccw(GLutil_Vec2f v, float theta)
{
        float sine   = sinf(theta);
        float cosine = cosf(theta);
        
        GLutil_Vec2f v_rot = (GLutil_Vec2f) {
            .x = + v.x * cosine + v.y * sine,
            .y = + v.y * cosine - v.x * sine,
        };

        return v_rot;
}

GLutil_Vec2f GLutil_Vec2f_rotcw(GLutil_Vec2f v, float theta)
{
        float sine   = sinf(theta);
        float cosine = cosf(theta);
        
        GLutil_Vec2f v_rot = (GLutil_Vec2f) {
            .x = + v.x * cosine - v.y * sine,
            .y = + v.y * cosine + v.x * sine,
        };

        return v_rot;
}

float GLutil_Vec2f_dot(GLutil_Vec2f a, GLutil_Vec2f b)
{
    return  a.x * b.x + a.y * b.y;
}

GLutil_Vec2f GLutil_Vec2f_apply_mat2x2(GLutil_Vec2f v, Mat2x2f A)
{
    //  | A00  A01 | * | vx | =>  | dot( {A00, A01}, v ) |
    //  | A10  A11 |   | vy |     | dot( {A10, A11}, v ) |
    return (GLutil_Vec2f) {
        .x = GLutil_Vec2f_dot(A.rows[0], v),
        .y = GLutil_Vec2f_dot(A.rows[1], v),
    };
}

GLutil_Vec2f GLutil_Vec2f_scale(GLutil_Vec2f v, float sx, float sy)
{
    return GLutil_Vec2f_apply_mat2x2(v, (Mat2x2f) {
            ._11 = sx, ._12 = 00,
            ._21 = 00, ._22 = sy,
    });
}


GLutil_Vec3f GLutil_Vec3f_add(GLutil_Vec3f a, GLutil_Vec3f b)
{
    return (GLutil_Vec3f) {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
}


GLutil_Vec3f GLutil_Vec3f_sub(GLutil_Vec3f a, GLutil_Vec3f b)
{
    return (GLutil_Vec3f) {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
}


float GLutil_Vec3f_sqlength(GLutil_Vec3f v)
{
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

float GLutil_Vec3f_length(GLutil_Vec3f v)
{
    return sqrt(GLutil_Vec3f_sqlength(v));
}

GLutil_Vec3f GLutil_Vec3f_div(GLutil_Vec3f v, float d)
{
    assert(d != 0);
    return (GLutil_Vec3f) {
        .x = v.x / d,
        .y = v.y / d,
        .z = v.z / d,
    };
}

GLutil_Vec3f GLutil_Vec3f_normalize(GLutil_Vec3f v)
{
    return GLutil_Vec3f_div(v, GLutil_Vec3f_length(v));
}



GLutil_Vec3f GLutil_Vec3f_scale(GLutil_Vec3f v, float s)
{
    return (GLutil_Vec3f) {
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s,
    };
}

float GLutil_Vec3f_dot(GLutil_Vec3f a, GLutil_Vec3f b)
{
    return a.x * b.x  + a.y * b.y + a.z * b.z ;
}

GLutil_Vec3f GLutil_Vec3f_cross(GLutil_Vec3f a, GLutil_Vec3f b)
{
    return (GLutil_Vec3f){
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x,
    };
}


GLutil_Mat2x2f GLutil_Mat2x2f_mul(GLutil_Mat2x2f A, GLutil_Mat2x2f B)
{
    //                                    / row_0  /  / col_0  /          / row_0  /  / col_1  /  
    // | A00 A01 | * | B00 B01 | = | dot( {A00, A01}, {B00, B10} )   dot( {A00, A01}, {B01, B11} )
    // | A10 A11 |   | B10 B11 |   | dot( {A10, A11}, {B00, B10} )   dot( {A10, A11}, {B01, B11} )
    //                                    / row_1 /   / col_0  /          / row_1 /   / col_1  /  
    return (GLutil_Mat2x2f) {
        ._11 = GLutil_Vec2f_dot( A.rows[0], (GLutil_Vec2f) {B._11, B._21}), ._12 = GLutil_Vec2f_dot( A.rows[0], (GLutil_Vec2f) {B._12, B._22}),
        ._21 = GLutil_Vec2f_dot( A.rows[1], (GLutil_Vec2f) {B._11, B._21}), ._22 = GLutil_Vec2f_dot( A.rows[1], (GLutil_Vec2f) {B._12, B._22}),
    };
}

GLutil_Mat3x3f GLutil_Mat3x3f_mul(GLutil_Mat3x3f A, GLutil_Mat3x3f B)
{

    GLutil_Mat3x3f m;
    m._11 = A._11*B._11 + A._12*B._21 + A._13*B._31;
    m._12 = A._11*B._12 + A._12*B._22 + A._13*B._32;
    m._13 = A._11*B._13 + A._12*B._23 + A._13*B._33;
    m._21 = A._21*B._11 + A._22*B._21 + A._23*B._31;
    m._22 = A._21*B._12 + A._22*B._22 + A._23*B._32;
    m._23 = A._21*B._13 + A._22*B._23 + A._23*B._33;
    m._31 = A._31*B._11 + A._32*B._21 + A._33*B._31;
    m._32 = A._31*B._12 + A._32*B._22 + A._33*B._32;
    m._33 = A._31*B._13 + A._32*B._23 + A._33*B._33;
    return m;


}

GLutil_Mat4x4f GLutil_Mat4x4f_mul(GLutil_Mat4x4f A, GLutil_Mat4x4f B)
{
    // from: https://github.com/tsoding/la/blob/master/la.h
    GLutil_Mat4x4f m;
    m._11 = A._11*B._11 + A._12*B._21 + A._13*B._31 + A._14*B._41;
    m._12 = A._11*B._12 + A._12*B._22 + A._13*B._32 + A._14*B._42;
    m._13 = A._11*B._13 + A._12*B._23 + A._13*B._33 + A._14*B._43;
    m._14 = A._11*B._14 + A._12*B._24 + A._13*B._34 + A._14*B._44;
    m._21 = A._21*B._11 + A._22*B._21 + A._23*B._31 + A._24*B._41;
    m._22 = A._21*B._12 + A._22*B._22 + A._23*B._32 + A._24*B._42;
    m._23 = A._21*B._13 + A._22*B._23 + A._23*B._33 + A._24*B._43;
    m._24 = A._21*B._14 + A._22*B._24 + A._23*B._34 + A._24*B._44;
    m._31 = A._31*B._11 + A._32*B._21 + A._33*B._31 + A._34*B._41;
    m._32 = A._31*B._12 + A._32*B._22 + A._33*B._32 + A._34*B._42;
    m._33 = A._31*B._13 + A._32*B._23 + A._33*B._33 + A._34*B._43;
    m._34 = A._31*B._14 + A._32*B._24 + A._33*B._34 + A._34*B._44;
    m._41 = A._41*B._11 + A._42*B._21 + A._43*B._31 + A._44*B._41;
    m._42 = A._41*B._12 + A._42*B._22 + A._43*B._32 + A._44*B._42;
    m._43 = A._41*B._13 + A._42*B._23 + A._43*B._33 + A._44*B._43;
    m._44 = A._41*B._14 + A._42*B._24 + A._43*B._34 + A._44*B._44;
    return m;
}



/*******************************************************************************
 *                     DESENHAR FORMAS GEOMÉTRICAS                             *
 *******************************************************************************/


void GLutil_line(GLutil_Vec2f a, GLutil_Vec2f b, GLutil_Color clr, float w)
{
     glLineWidth(w);
     glColor4ub( clr.red, clr.green, clr.blue, clr.alpha );
     glBegin( GL_LINES );
     glVertex2f( a.x, a.y );
     glVertex2f( b.x, b.y );
     glEnd();
}


#ifndef GLUTIL_CIRCLE_PRECISION
#define GLUTIL_CIRCLE_PRECISION 100.0f
#endif // GLUTIL_CIRCLE_PRECISION
void GLutil_circle(GLutil_Vec2f center, int radius, GLutil_Color color)
{

    float step = (2 * M_PI) / GLUTIL_CIRCLE_PRECISION;

    glBegin( GL_TRIANGLE_FAN );
    glColor4ub( color.red, color.green, color.blue, color.alpha );

    glVertex2f(center.x, center.y);

    GLutil_Vec2f cur = (GLutil_Vec2f) {
        .x = center.x,
        .y = center.y + radius,
    };

    for (float angle = 0; angle <= 2 * M_PI; angle += step) {
        cur.x = center.x + ( radius * sinf(angle) );
        cur.y = center.y + ( radius * cosf(angle) );
        glVertex2f(cur.x, cur.y);
    }

    GLutil_Vec2f last = (GLutil_Vec2f) {
        .x = center.x,
        .y = center.y + radius,
    };
    glVertex2f(last.x, last.y);

    glEnd();
}


void GLutil_rect(GLutil_Vec2f topleft, float w, float h, GLutil_Color clr)
{
    glColor4ub( clr.red, clr.green, clr.blue, clr.alpha );
    glBegin( GL_TRIANGLE_STRIP );
    glVertex2f( topleft.x,      topleft.y     );
    glVertex2f( topleft.x,      topleft.y + h );
    glVertex2f( topleft.x + w , topleft.y     );
    glVertex2f( topleft.x + w , topleft.y + h );
    glEnd();
}



 /*******************************************************************************
 *                             SHADERS                                         *
 *******************************************************************************/

GLuint GLutil_load_shader(const char* name, GLutil_ShaderType type)
{
    GLuint shader_id = glCreateShader(type); GLUTIL_CHECK_GL_ERROR();

    FILE* f = fopen(name, "r");
    if (f == NULL) {
        printf(
            "%s:%d:ERROR: load_shader: failed to open file '%s': %s\n",
            __FILE_NAME__, __LINE__, name, strerror(errno)
        );
        // if error => sb->data == NULL 
        return (GLuint)-1;
    }

    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = (char*)calloc((n + 1), sizeof(char));
    fread(data, sizeof(char), n, f);
    fclose(f);

    const char* shader_src = data;
    glShaderSource(shader_id, 1, &shader_src , NULL); GLUTIL_CHECK_GL_ERROR();
    free(data);
    
    glCompileShader(shader_id); GLUTIL_CHECK_GL_ERROR();

    return shader_id;
}


void GLutil_default_shaders(GLutil_ObjData* obj)
{
    if (obj->shader_program != (GLuint)-1) {
        glDeleteProgram(obj->shader_program); GLUTIL_CHECK_GL_ERROR();
    }

    GLuint shader = glCreateProgram(); GLUTIL_CHECK_GL_ERROR();
    GLuint vs_id  = GLutil_load_shader(DEFAULT_VERT_SHADER_PATH, GLUTIL_VERT_SHADER); GLUTIL_CHECK_GL_ERROR();
    GLuint fs_id  = GLutil_load_shader(DEFAULT_FRAG_SHADER_PATH, GLUTIL_FRAG_SHADER); GLUTIL_CHECK_GL_ERROR();

    glAttachShader(shader, vs_id); GLUTIL_CHECK_GL_ERROR();
    glAttachShader(shader, fs_id); GLUTIL_CHECK_GL_ERROR();
    glLinkProgram(shader); GLUTIL_CHECK_GL_ERROR();
    glDeleteShader(vs_id); GLUTIL_CHECK_GL_ERROR();
    glDeleteShader(fs_id); GLUTIL_CHECK_GL_ERROR();

    // @TODO: error handling
    obj->shader_program = shader;
    printf("Generated Default Shader with id = %u\n", shader);
}

/*******************************************************************************
 *                               CAMERA                                        *
 ******************************************************************************/

GLutil_Mat3x3f GLutil_model_matrix(GLutil_Camera cam)
{
    return (GLutil_Mat3x3f) {
        .data = {                                    
            1/500.0f, 0,        0, 
            0,        1/500.0f, 0, 
            0,        0,        1/500.0f    
        }
    };
}

GLutil_Mat4x4f GLutil_view_matrix(GLutil_Camera cam)
{

    GLutil_Vec3f dir = GLutil_Vec3f_sub(cam.pos, cam.target);
    dir = GLutil_Vec3f_normalize(dir);

    GLutil_Vec3f right = GLutil_Vec3f_cross(cam.up, dir);
    right = GLutil_Vec3f_normalize(right);

    GLutil_Vec3f up = cam.up;

    // from: https://github.com/lukeflima/Segundo-Trabalho/blob/master/Segundo-Trabalho/mygl.h 
    GLutil_Mat4x4f lukeat = (GLutil_Mat4x4f) {
        .data = {
            right.x, right.y, right.z, 0,
            up.x,    up.y,    up.z, 0,
            dir.x,   dir.y,   dir.z,   0,
            0,       0,       0,       1,
        },
    };

    GLutil_Mat4x4f pos = (GLutil_Mat4x4f) {
        .data = {
            1.0f, 0.0f,  0.0f,  -cam.pos.x,
            0.0f, 1.0f,  0.0f,  -cam.pos.y,
            0.0f, 0.0f,  1.0f,  -cam.pos.z,
            0.0f, 0.0f,  0.0f,   1.0f,
        }
    };


    return Mat4x4f_mul(lukeat, pos);

}

GLutil_Mat4x4f GLutil_projection_matrix(GLutil_Camera cam, float aspectRatio)
{
    const float DEG2RAD = (float)M_PI / 180.0f;
    float tangent = tanf(cam.fovX/2.0f * DEG2RAD);    // tangent of half fovX
    return (GLutil_Mat4x4f) { 
        .data = { 
            1.0f/(aspectRatio*tangent), 0.0f,           0.0f,                     0.0f,
            0.0f,                       1.0f /tangent,  0.0f,                     0.0f,
            0.0f,                       0.0f,           -(cam.far + cam.near)/(cam.far-cam.near), -(2*cam.far*cam.near)/(cam.far-cam.near),
            0.0f,                       0.0f,           -1.0f,                    0.0f,
        }
    };
}

/* @TODO: checar comentário na declaração da assinatura de projection_matrix
GLutil_Mat4x4f GLutil_projection_matrix(GLutil_Camera cam)
{
    return GLutil_projection_matrix_(cam, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
}
*/

/*******************************************************************************
 *                               3D MODEL OBJ                                  *
 ******************************************************************************/

CarrStringView NORMAL_TAG  = (CarrStringView) { .data = "vn", .len  = 2  };
CarrStringView TEXTURE_TAG = (CarrStringView) { .data = "vt", .len  = 2  };
CarrStringView SMOOTH_TAG  = (CarrStringView) { .data = "s",  .len  = 1  };
CarrStringView OBJECT_TAG  = (CarrStringView) { .data = "o",  .len  = 1  };
CarrStringView VERTEX_TAG  = (CarrStringView) { .data = "v",  .len  = 1  };
CarrStringView FACE_TAG    = (CarrStringView) { .data = "f",  .len  = 1  };


typedef struct {
    float* items;
    size_t len;
    size_t cap;
} VecFloats;


ObjData GLutil_load_obj_file(const char* file_path)
{
    ObjData obj = {0};

    Vertices verts = {0};
    carr_vec_init(&verts);

    Faces faces = {0};
    carr_vec_init(&faces);


    CarrStringBuilder sb = carr_sb_from_file(file_path);
    if (sb.data == NULL) exit(1);
    CarrStringView file = carr_sv_from_sb(sb);

    VecFloats vs = {0};  // temp vec for vertices coordinates
    carr_vec_init(&vs);

    VecFloats vts = {0};  // temp vec for textures coordinates
    carr_vec_init(&vts);

    Faces v_idxs = {0};   // temp vec for vertices indexes for faces
    carr_vec_init(&v_idxs);

    Faces vt_idxs = {0};  // temp vec for texture indixes for faces
    carr_vec_init(&vt_idxs);


    while (file.len > 0) {
        CarrStringView line = carr_sv_chop_line(&file);
        if (line.len == 0 || carr_sv_first(&line) == '#')
            continue;

        CarrStringView tag = carr_sv_chop_by_space(&line); 

        if (carr_sv_is_equal(tag, NORMAL_TAG) || 
            carr_sv_is_equal(tag, OBJECT_TAG) || carr_sv_is_equal(tag, SMOOTH_TAG)) {
            printf("[WARNING] Vertex Data not supported: '%.*s'\n", (int)tag.len, tag.data);
        } else if ( carr_sv_is_equal(tag, VERTEX_TAG) ) {

            char linebuf[100] = {0};
            assert(line.len < 100);
            memcpy(linebuf, line.data, sizeof(char) * line.len);

            float x, y, z;
            sscanf(linebuf, "%f %f %f", &x, &y, &z);


            carr_vec_append(&vs, x);
            carr_vec_append(&vs, y);
            carr_vec_append(&vs, z);

        } else if ( carr_sv_is_equal(tag, TEXTURE_TAG) ) {

            char linebuf[100] = {0};
            assert(line.len < 100);
            memcpy(linebuf, line.data, sizeof(char) * line.len);

            float u, v;
            sscanf(linebuf, "%f %f", &u, &v);

            carr_vec_append(&vts, u);
            carr_vec_append(&vts, v);

        } else if ( carr_sv_is_equal(tag, FACE_TAG) ) {

            char linebuf[100] = {0};
            assert(line.len < 100);
            memcpy(linebuf, line.data, sizeof(char) * line.len);

            char buf1[35];
            char buf2[35];
            char buf3[35];
            
            sscanf(linebuf, "%s %s %s", buf1, buf2, buf3);

            char* buffers[3] = { buf1, buf2, buf3 };

            for (int i = 0; i < 3; ++i) {

                char* buf = buffers[i];

                CarrStringView buf_sv = carr_sv_from_cstr(buf);
                CarrStringView idx = carr_sv_chop_by_delim(&buf_sv, '/');
                int idx_int = carr_sv_parse_int(&idx);
                carr_vec_append(&v_idxs, idx_int - 1);
                if (buf_sv.len > 0) {
                    idx = carr_sv_chop_by_delim(&buf_sv, '/');
                    idx_int = carr_sv_parse_int(&idx);
                    carr_vec_append(&vt_idxs, idx_int - 1);
                    if (buf_sv.len > 0) {
                        idx = carr_sv_chop_by_delim(&buf_sv, '/');
                        idx_int = carr_sv_parse_int(&idx);
                        // @TODO: criar mecanismo para lidar com normais
                        // @TODO: coloca idx_int no vec de índices para normais
                    }
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

    /* @TODO: add debug logging flags?
    printf("VIDXS   => { data: %p, len: %lu, cap: %lu }\n", v_idxs.items, v_idxs.len, v_idxs.cap);
    printf("VT_IDXS => { data: %p, len: %lu, cap: %lu }\n", vt_idxs.items, vt_idxs.len, vt_idxs.cap);

    printf(" ARRAY DE COORDENADAS \n");
    for (int i = 0; i < vs.len; ++i) {
        printf("[%d] %f\n", i, vs.items[i]);
    }
    */



    for (int i = 0; i < v_idxs.len; ++i) {
        int vidx = v_idxs.items[i] * 3;
        

        float x = vs.items[vidx + 0];
        float y = vs.items[vidx + 1];
        float z = vs.items[vidx + 2];

        /*
        printf("----\n");
        printf("VERTEX IDX = %d\n", vidx);
        printf(" x = %f, y = %f, z = %f \n", x, y, z);
        */

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

        carr_vec_append(&verts, vertex);
        carr_vec_append(&faces, verts.len - 1);

        /*
        printf("  FINAL FACE IDX = %lu\n", verts.len - 1);
        printf("-----\n");
        */
        // __asm__("int3");

    }

    carr_vec_free(&vs);
    carr_vec_free(&vts);
    carr_vec_free(&v_idxs);
    carr_vec_free(&vt_idxs);
    carr_sb_free(&sb);


    obj.vertices   = verts;
    obj.faces      = faces;

    default_shaders(&obj);
    GLutil_load_obj_to_GPU(&obj);
    
    return obj;
}

void GLutil_load_obj_to_GPU(GLutil_ObjData* obj)
{
    // create GPU buffers for vertex and indexes data
    glGenVertexArrays(1, &obj->vao_id); // VAO => Stores Vertex Attributes Layout and overall configuration of other buffers
    glGenBuffers(1,      &obj->vbo_id); // VBO => Stores Vertex Attributes Raw Data
    glGenBuffers(1,      &obj->ebo_id); // EBO => Stores Index Data that points to the VBO as to define the vertices of a face

    // bind the Vertex Attributes Array, which holds all the 
    // configurations for the other buffers 
    // (the Vertex Attributes corresponds to the GLutil_Vertex struct)
    glBindVertexArray(obj->vao_id);

    // bind and load the vertices' coordinates 
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLutil_Vertex)*obj->vertices.len, obj->vertices.items, GL_STATIC_DRAW);

    // bind and load the faces' vertices indexes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*obj->faces.len, obj->faces.items, GL_STATIC_DRAW );

    // create an individual Attribute Pointer associated with each field 
    // in the GLutil_Vertex struct so that it's usable from the shaders
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*obj->vertices.items), (void*)offsetof(GLutil_Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(*obj->vertices.items), (void*)offsetof(GLutil_Vertex, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(*obj->vertices.items), (void*)offsetof(GLutil_Vertex, tex_pos));
    glEnableVertexAttribArray(2);

    // bind default buffer and vertex array as a cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

}



GLuint GLutil_load_texture(const char* texture_name)
{
    GLuint texture_id;
    int img_width, img_height, n;
    unsigned char *data = stbi_load(texture_name, &img_width, &img_height, &n, 0);

    if (data == NULL)  {
        PRINT_ERROR("stbi_load failed\n");
        exit(1);
    }


    glGenTextures(1, &texture_id); GLUTIL_CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texture_id); GLUTIL_CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); GLUTIL_CHECK_GL_ERROR();  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); GLUTIL_CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); GLUTIL_CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GLUTIL_CHECK_GL_ERROR();

    // glActiveTexture(GL_TEXTURE0);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        img_width, img_height,
        0, GL_RGB, GL_UNSIGNED_BYTE,
        data 
    ); GLUTIL_CHECK_GL_ERROR();
    glGenerateMipmap(GL_TEXTURE_2D); GLUTIL_CHECK_GL_ERROR();

    stbi_image_free(data);

    return texture_id;
}

GLutil_Model GLutil_model_from_obj(GLutil_ObjData* obj)
{
    return (GLutil_Model) {
        .scale = 10000.0f,  // @TODO: encontrar valores mais razoáveis de escala padrão lol
        .world_pos = { 0.0f, 0.0f, 0.0f },
        .geometry = obj,
        .texture_id = -1
    };
}


void GLutil_render_model(GLutil* glutil, GLutil_Model* model)
{
    glUseProgram(model->geometry->shader_program); CHECK_GL_ERROR();
    glBindVertexArray(model->geometry->vao_id); CHECK_GL_ERROR();

    GLuint model_loc = glGetUniformLocation(model->geometry->shader_program, "model_mat");
    CHECK_GL_ERROR();
    glUniformMatrix3fv(model_loc, 1, GL_FALSE, GLutil_model_matrix(*glutil->camera).data);
    CHECK_GL_ERROR();

    GLuint view_loc = glGetUniformLocation(model->geometry->shader_program, "view_mat");
    CHECK_GL_ERROR();
    glUniformMatrix4fv(view_loc, 1, GL_TRUE, GLutil_view_matrix(*glutil->camera).data);
    CHECK_GL_ERROR();

    GLutil_Mat4x4f projection = GLutil_projection_matrix(*glutil->camera, glutil->winwidth / (float)glutil->winheight);
    GLuint proj_loc = glGetUniformLocation(model->geometry->shader_program, "proj_mat");
    CHECK_GL_ERROR();
    glUniformMatrix4fv(proj_loc, 1, GL_TRUE, projection.data);
    CHECK_GL_ERROR();

    GLuint t_loc = glGetUniformLocation(model->geometry->shader_program, "t"); CHECK_GL_ERROR();
    float t_secs = glutil->cur_t_nanos / 1000.0f / 1000.0f / 1000.0f;  // 1 sec = 10^9 nanosecs
    glUniform1f(t_loc, t_secs); CHECK_GL_ERROR();

    GLuint scale_loc = glGetUniformLocation(model->geometry->shader_program, "scale"); CHECK_GL_ERROR();
    glUniform1f(scale_loc, model->scale); CHECK_GL_ERROR();

    const float fs[3] = { model->world_pos.x, model->world_pos.y, model->world_pos.z };
    GLuint wpos_loc = glGetUniformLocation(model->geometry->shader_program, "world_pos"); CHECK_GL_ERROR();
    glUniform3fv(wpos_loc, 1, fs); CHECK_GL_ERROR();

    GLuint texture_loc = glGetUniformLocation(model->geometry->shader_program, "the_texture"); CHECK_GL_ERROR();
    glUniform1i(texture_loc, 0); CHECK_GL_ERROR();
    
    GLuint has_texture_loc = glGetUniformLocation(model->geometry->shader_program, "has_texture"); CHECK_GL_ERROR();
    if ( model->texture_id == (GLuint)-1 ) {
        glUniform1i(has_texture_loc, false); CHECK_GL_ERROR();
    } else {
        glUniform1i(has_texture_loc, true); CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, model->texture_id); CHECK_GL_ERROR();
    }

    glDrawElements(GL_TRIANGLES, model->geometry->faces.len, GL_UNSIGNED_INT, 0); CHECK_GL_ERROR();
    // printf("[DRAW] Error: %s\n", glErrorNames[glGetError()]);
}


#endif // GLUTIL_H_

