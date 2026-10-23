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

#ifndef GLUTIL_FORCE_PREFIX

#define MAP    GLUTIL_MAP
#define RAND   GLUTIL_RAND
#define COLOR  GLUTIL_COLOR
#define BLACK  GLUTIL_BLACK

#define Color   GLutil_Color
#define Vec2f   GLutil_Vec2f

#define init    GLutil_init
#define destroy GLutil_destroy

#define circle   GLutil_circle
#define line     GLutil_line
#define rect     GLutil_rect
#define ortho2D  GLutil_ortho2D
#define clear_bg GLutil_clear_bg

#define Vec2f_add      GLutil_Vec2f_add
#define Vec2f_subtract GLutil_Vec2f_subtract
#define Vec2f_sqlength GLutil_Vec2f_sqlength
#define Vec2f_length   GLutil_Vec2f_length
#define Vec2f_rotccw   GLutil_Vec2f_rotccw
#define Vec2f_rotcw    GLutil_Vec2f_rotcw


#define get_time_nanos    GLutil_get_time_nanos
#define begin_frame       GLutil_begin_frame
#define frame_delta_nanos GLutil_frame_delta_nanos
#define end_frame         GLutil_end_frame
#define present           GLutil_present
#define set_target_fps    GLutil_set_target_fps
#define next_event        GLutil_next_event

#define flush_shaders      GLutil_flush_shaders
#define load_shader        GLutil_load_shader
#define use_shaders        GLutil_use_shaders
#define stop_using_shaders GLutil_stop_using_shaders

#define load_vertices_and_faces GLutil_load_vertices_and_faces
#define draw                    GLutil_draw

#endif // GLUTIL_INCLUDE_PREFIX

#define GLUTIL_MAP(x, src_min, src_max, dst_min, dst_max) ( (dst_min) + ( ( ( (x) - (src_min) ) * ( (dst_max) - (dst_min) ) ) / ( (src_max) - (src_min) ) ))
#define GLUTIL_RAND(min, max) MAP((float)rand(), 0.0f, (float)RAND_MAX, (float)(min), (float)(max))

#define GLUTIL_COLOR(r, g, b) (Color){ .red = (r), .green = (g), .blue = (b), .alpha = 255 }
#define GLUTIL_BLACK COLOR(0, 0, 0)

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


typedef struct GLutil {
    Window       window;
    Display*     display;
    GLXContext   ctx;
    XVisualInfo* vi;
    GLXFBConfig  fbc;

    uint64_t     prev_t_nanos;    // timestamp em nanosegundos do último frame
    uint64_t     cur_t_nanos;     // timestamp em nanosegundos do frame atual
    uint64_t     dt_nanos;        // delta time em nanosegundos entre os dois últimos frames
    uint64_t     target_sleep_us; // quantidade de microsegundos de delay para atingir o FPS alvo

} GLutil;


typedef struct {
    unsigned char red;  
    unsigned char green;
    unsigned char blue; 
    unsigned char alpha;
} GLutil_Color;


typedef struct  {
    float x;
    float y;
} GLutil_Vec2f;


 typedef enum {
    GLUTIL_VERT_SHADER,
    GLUTIL_FRAG_SHADER,
} GLutil_ShaderType;


void GLutil_init(GLutil* glutil, unsigned int w, unsigned int h);            // inicializa Display, Window e o OpenGLContext. e mais alguns estados internos
void GLutil_destroy(GLutil* glutil);                                         // desaloca e fecha todos os recursos alocados por GLutil.h


void GLutil_circle(GLutil_Vec2f center, int radius, GLutil_Color color);     // desenha um círculo através de GL_TRIANGLE_FAN, o nível de precisão do círculo pode ser alterado pelo macro GLUTIL_CIRCLE_PRECISION
void GLutil_line(GLutil_Vec2f a, GLutil_Vec2f b, GLutil_Color clr, float w);
void GLutil_rect(GLutil_Vec2f topleft, float w, float h, GLutil_Color clr);  
void GLutil_ortho2D(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom);
void GLutil_clear_bg( Color color );


GLutil_Vec2f GLutil_Vec2f_add(GLutil_Vec2f a, GLutil_Vec2f b);
GLutil_Vec2f GLutil_Vec2f_subtract(GLutil_Vec2f a, GLutil_Vec2f b);
float GLutil_Vec2f_sqlength(GLutil_Vec2f v);
float GLutil_Vec2f_length(GLutil_Vec2f v);
GLutil_Vec2f GLutil_Vec2f_rotccw(GLutil_Vec2f v, float theta);
GLutil_Vec2f GLutil_Vec2f_rotcw(GLutil_Vec2f v, float theta);


uint64_t GLutil_get_time_nanos(void);
uint64_t GLutil_begin_frame(GLutil *glutil);
uint64_t GLutil_frame_delta_nanos(GLutil *glutil);
void GLutil_end_frame(GLutil *glutil);
void GLutil_present(GLutil* glutil);
void GLutil_set_target_fps(GLutil* glutil, float fps);
bool GLutil_next_event(GLutil* glutil, XEvent* event);

void GLutil_flush_shaders();
void GLutil_load_shader(const char* name, GLutil_ShaderType type);
bool GLutil_use_shaders();
void GLutil_stop_using_shaders();

void GLutil_load_vertices_and_faces(float* vertices, int vertices_count,
                         unsigned int* faces, int faces_count);
void GLutil_draw();

// char* glErrorNames[] = {
//     [0] = "GL_NO_ERROR ",
//     [0x0500] = "GL_INVALID_ENUM",
//     [0x0501] = "GL_INVALID_VALUE",
//     [0x0502] = "GL_INVALID_OPERATION",
//     [0x0503] = "GL_STACK_OVERFLOW",
//     [0x0504] = "GL_STACK_UNDERFLOW",
//     [0x0505] = "GL_OUT_OF_MEMORY",
// };


#endif // GLUTIL_H_



/**************************** FIM DO HEADER ***********************************/



// #define GLUTIL_IMPLEMENTATION 
#ifdef GLUTIL_IMPLEMENTATION

// char* GL_ERROR_NAMES[] = {
//     [0] = "GL_NO_ERROR ",
//     [0x0500] = "GL_INVALID_ENUM",
//     [0x0501] = "GL_INVALID_VALUE",
//     [0x0502] = "GL_INVALID_OPERATION",
//     [0x0503] = "GL_STACK_OVERFLOW",
//     [0x0504] = "GL_STACK_UNDERFLOW",
//     [0x0505] = "GL_OUT_OF_MEMORY",
// };


char* INPUT_TYPE_NAMES[] = {
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
 *  INICIALIZAÇÃO DO OPENGL                                                    *
 *  (from: https://github.com/ColleagueRiley/OpenGL-Context-Creation)          *
 ******************************************************************************/

typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

typedef void ( *glGenBuffersProc ) ( GLsizei n,     GLuint *buffers                                 );
typedef void ( *glBindBufferProc ) ( GLenum target, GLuint  buffer                                  );
typedef void ( *glBufferDataProc ) ( GLenum target, GLsizeiptr size, const void *data, GLenum usage );

typedef GLuint ( *glCreateShaderProc  ) ( GLenum type                                                                    );
typedef void   ( *glShaderSourceProc  ) ( GLuint shader,  GLsizei count, const GLchar *const*string, const GLint *length );
typedef void   ( *glCompileShaderProc ) ( GLuint shader                                                                  );
typedef void   ( *glAttachShaderProc  ) ( GLuint program, GLuint shader                                                  );
typedef void   ( *glDeleteShaderProc  ) ( GLuint shader                                                                  );

typedef GLuint ( *glCreateProgramProc ) ( void           );
typedef void   ( *glLinkProgramProc   ) ( GLuint program );
typedef void   ( *glUseProgramProc    ) ( GLuint program );
typedef void   ( *glDeleteProgramProc ) ( GLuint program );

typedef void ( *glVertexAttribPointerProc     ) ( GLuint index, GLint size,     GLenum type, GLboolean normalized, GLsizei stride, const void *pointer );
typedef void ( *glEnableVertexAttribArrayProc ) ( GLuint index                                                                                         );
typedef void ( *glGenVertexArraysProc         ) ( GLsizei n,    GLuint *arrays                                                                         );
typedef void ( *glBindVertexArrayProc         ) ( GLuint array                                                                                         );

typedef void ( *glGetProgramivProc ) ( GLuint program, GLenum pname, GLint *params );

typedef GLint ( *glGetUniformLocationProc ) ( GLuint program, const GLchar *name );
typedef void  ( *glUniform1fProc          ) ( GLint location, GLfloat v0         );


typedef uint8_t GL_profile; 
enum  { 
    GL_CORE = 0,  
    GL_COMPATIBILITY  
};
int32_t majorVersion = 0, minorVersion = 0;
Bool profile = GL_CORE;

static const int32_t STENCIL = 0, SAMPLES = 0, STEREO = GL_FALSE, AUX_BUFFERS = 0, DOUBLE_BUFFER = 1;


glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
glGenBuffersProc               glGenBuffers;
glBindBufferProc               glBindBuffer;
glBufferDataProc               glBufferData;
glCreateShaderProc             glCreateShader;
glShaderSourceProc             glShaderSource;
glCompileShaderProc            glCompileShader;
glAttachShaderProc             glAttachShader;
glDeleteShaderProc             glDeleteShader;
glCreateProgramProc            glCreateProgram;
glLinkProgramProc              glLinkProgram;
glUseProgramProc               glUseProgram;
glDeleteProgramProc            glDeleteProgram;
glVertexAttribPointerProc      glVertexAttribPointer;
glEnableVertexAttribArrayProc  glEnableVertexAttribArray;
glGenVertexArraysProc          glGenVertexArrays;
glBindVertexArrayProc          glBindVertexArray;
glGetUniformLocationProc       glGetUniformLocation;
glUniform1fProc                glUniform1f;
glGetProgramivProc             glGetProgramiv;


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
    
    size_t index = (sizeof(attribs) / sizeof(attribs[0])) - 13;

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
    // @TODO: Xmacro this motherfucker
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*) "glXCreateContextAttribsARB");
    glGenBuffers = (glGenBuffersProc) glXGetProcAddressARB((GLubyte*)"glGenBuffers");
    glBindBuffer = (glBindBufferProc) glXGetProcAddressARB((GLubyte*)"glBindBuffer");
    glBufferData = (glBufferDataProc) glXGetProcAddressARB((GLubyte*)"glBufferData");

    glShaderSource  = (glShaderSourceProc)  glXGetProcAddressARB((GLubyte*)"glShaderSource");
    glCompileShader = (glCompileShaderProc) glXGetProcAddressARB((GLubyte*)"glCompileShader");
    glAttachShader  = (glAttachShaderProc)  glXGetProcAddressARB((GLubyte*)"glAttachShader");

    glLinkProgram = (glLinkProgramProc) glXGetProcAddressARB((GLubyte*)"glLinkProgram");
    glUseProgram  = (glUseProgramProc)  glXGetProcAddressARB((GLubyte*)"glUseProgram");
    glDeleteProgram  = (glDeleteProgramProc)  glXGetProcAddressARB((GLubyte*)"glDeleteProgram");

    glVertexAttribPointer     = (glVertexAttribPointerProc)     glXGetProcAddressARB((GLubyte*)"glVertexAttribPointer");
    glEnableVertexAttribArray = (glEnableVertexAttribArrayProc) glXGetProcAddressARB((GLubyte*)"glEnableVertexAttribArray");
    glGenVertexArrays         = (glGenVertexArraysProc)         glXGetProcAddressARB((GLubyte*)"glGenVertexArrays");

    glCreateShader    = (glCreateShaderProc)    glXGetProcAddressARB((GLubyte*)"glCreateShader");    
    glCreateProgram   = (glCreateProgramProc)   glXGetProcAddressARB((GLubyte*)"glCreateProgram");   
    glDeleteShader    = (glDeleteShaderProc)    glXGetProcAddressARB((GLubyte*)"glDeleteShader");    
    glBindVertexArray = (glBindVertexArrayProc) glXGetProcAddressARB((GLubyte*)"glBindVertexArray");  

    glGetUniformLocation = (glGetUniformLocationProc) glXGetProcAddressARB((GLubyte*)"glGetUniformLocation");    
    glUniform1f          = (glUniform1fProc)          glXGetProcAddressARB((GLubyte*)"glUniform1f");    

    glGetProgramiv = (glGetProgramivProc)    glXGetProcAddressARB((GLubyte*)"glGetProgramiv");    


    GLutil_make_display(glutil);
    GLutil_make_fbc_and_vi(glutil);
    GLutil_make_window(glutil, w, h);
    GLutil_make_context(glutil);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    GLutil_ortho2D(0, w, 0, h);

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


void GLutil_ortho2D(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom)
{
    glOrtho(left, right, bottom, top, -1, 1);
}

void GLutil_clear_bg( Color color )
{
    glClearColor(color.red / 255.0f,  color.green / 255.0f, color.blue / 255.0f, color.alpha / 255.0f );
    glClear( GL_COLOR_BUFFER_BIT );
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

GLutil_Vec2f GLutil_Vec2f_subtract(GLutil_Vec2f a, GLutil_Vec2f b)
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

// @NOTE: change to vector
#define SHADERS_CAP 1024
int SHADERS_COUNT = 0;
GLuint SHADERS[SHADERS_CAP];
GLuint current_shader_program = -1;


void GLutil_flush_shaders()
{
    for (int i = 0; i < SHADERS_COUNT; ++i) {
        glDeleteShader(SHADERS[i]);
    }
    SHADERS_COUNT = 0;
}

void GLutil_load_shader(const char* name, GLutil_ShaderType type)
{
    GLenum gl_type = type == GLUTIL_VERT_SHADER ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    GLuint shader_id = glCreateShader(gl_type);

    FILE* f = fopen(name, "r");
    if (f == NULL) {
        printf(
            "%s:%d:ERROR: load_shader: failed to open file '%s': %s\n",
            __FILE_NAME__, __LINE__, name, strerror(errno)
        );
        // if error => sb->data == NULL 
        return;
    }

    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* data = (char*)calloc((n + 1), sizeof(char));
    fread(data, sizeof(char), n, f);
    fclose(f);

    const char* shader_src = data;
    glShaderSource(shader_id, 1, &shader_src , NULL);
    free(data);

    glCompileShader(shader_id);

    assert(SHADERS_COUNT < SHADERS_CAP);
    SHADERS[SHADERS_COUNT++] = shader_id;
}


bool GLutil_use_shaders()
{
    int ret;
    GLuint shader_program = glCreateProgram();
    for (int i = 0; i < SHADERS_COUNT; ++i) {
        glAttachShader(shader_program, SHADERS[i]);

    }
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &ret );
    if (!ret) return false;
    glUseProgram(shader_program); // @TODO: error handling
    current_shader_program = shader_program;
    return true;
}

void GLutil_stop_using_shaders()
{
    flush_shaders();
    if (current_shader_program != -1) {
        glDeleteProgram(current_shader_program);
    }
}



/*******************************************************************************
 *                               BUFFERS                                       *
 ******************************************************************************/


GLuint current_vao = -1;
int current_ebo_count = -1;

void GLutil_load_vertices_and_faces(float* vertices, int vertices_count,
                         unsigned int* faces, int faces_count)
{
    GLuint buf_id, ebo_id, vao_id;
    glGenVertexArrays(1, &vao_id);
    glGenBuffers(1,      &buf_id);
    glGenBuffers(1,      &ebo_id);

    glBindVertexArray(vao_id);
    // printf("[VAO] Error: %s\n", glErrorNames[glGetError()]);

    glBindBuffer(GL_ARRAY_BUFFER, buf_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices_count, vertices, GL_STATIC_DRAW);
    // printf("[VBO] Error: %s\n", glErrorNames[glGetError()]);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*faces_count, faces, GL_STATIC_DRAW );
    // printf("[EBO] Error: %s\n", glErrorNames[glGetError()]);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // printf("[ATTRIB] Error: %s\n", glErrorNames[glGetError()]);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    // printf("[END] Error: %s\n", glErrorNames[glGetError()]);

    current_vao = vao_id;
    current_ebo_count = faces_count;
}

void GLutil_draw()
{
    glBindVertexArray(current_vao);
    glDrawElements(GL_TRIANGLES, current_ebo_count, GL_UNSIGNED_INT, 0);
    // printf("[DRAW] Error: %s\n", glErrorNames[glGetError()]);
}



#endif // GLUTIL_H_

