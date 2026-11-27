/*******************************
 *  LICENSES AT BOTTOM OF FILE *
 *******************************/

//  DEPENDÊNCIAS ///////////////////////////////////////////////////////////////
/*
ESSE ARQUIVO NÃO É AUTO-CONTIDO (por enquanto).
Por hora, esse header (glutil.h) depende de outras 3 header-only libs,
que precisam estar presentes numa pasta chamada 'include'.
Os três arquivos são:
  vec.h          (https://github.com/CarraraSoftware/carrlib/blob/main/vec.h)
  sv.h           (https://github.com/CarraraSoftware/carrlib/blob/main/sv.h)
  stb_image.h    (https://github.com/nothings/stb/blob/master/stb_image.h)

Ou seja, algo assim:

 <diretório do projeto>/
  │
  ├─ glutil.h
  │
  ├─ include/
  │    │
  │    ├─ vec.h
  │    ├─ sv.h
  │    ╰─ stb_image.h
  │
  ╰─ main.c (arquivo onde o glutil.h é incluído, ver exemplo abaixo)

*/
////////////////////////////////////////////////////////////////////////////////




/* Exemplo mínimo:
 *
 * file:// *********************  main.c ***************************************
 *
*/

#if 0
#define GLUTIL_IMPLEMENTATION // stb_style header only
#include "glutil.h"                                                           
                                                                              
int main(void) {                                                              
    GLutil glutil = {0};                     // aloca na stack um GLutil
    init(&glutil, 800, 600);                 // inicializa a struct que gerencia todo o estado da aplicação                                 
    set_target_fps(&glutil, 60.0f);

    immediate_mode_begin(&glutil);
    for (;;) {
        uint64_t dt = begin_frame(&glutil);  // processa todos os eventos do X11 e tudo mais que for necessário pra conseguir colocar os pixels na tela
            clear_bg(BLACK);
            circle(                          // desenhar um círcula usando o immediate_mode do OpenGL
                (Vec2f) { 100.0f, 100.0f },  //      centro do círculo             
                50,                          //      raio do círculo               
                COLOR( 255, 255, 255 )       //      cor do círculo                
            );                                                                
        end_frame(&glutil);                  // exibe os pixels (isto é, faz o swap dos buffers), atualiza o estado interno e qualquer outra coisa que precise antes do próximo frame
    }                                                                         
    immediate_mode_end(&glutil);
                                                                              
    destroy(&glutil);                                                         
    return 0;                                                                 
}                                                                             
#endif

/*
 * E então você compila o projeto com:
 * gcc -o main main.c -lX11 -lGL -lm
 *
 * e roda com:
 * ./main
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

// @NOTE: acho melhor forçar o prefixo somente durante os testes com o 
//        GLutil.h por meio de uma compile flag (e.g. -DCARR_VEC_FORCE_PREFIX) 
//        pra permitir que uma aplicação que usa o GLutil.h possa também usar o vec.h sem os prefixos
// #define CARR_VEC_FORCE_PREFIX 
#define CARR_VEC_IMPLEMENTATION
#include "include/vec.h"

// @NOTE: exatamente a mesma coisa que a nota acima, testar com -DCARR_SV_FORCE_PREFIX,
//        para permitir que os usuários escolham entre ter ou não o prefixo
#define CARR_SV_IMPLEMENTATION
#include "include/sv.h"





///////////////////////////////////////////////////////////////////////////////



/*******************************************************************************
 *                                                                             *
 *                        TYPE DEFINITIONS                                     *
 *                                                                             *
 ******************************************************************************/
// Todos os tipos aqui declarados com o prefixo GLutil_ podem ser acessados sem
// esse prefixo. O prefixo é incluído como um mecanismo que permita contornar 
// situações de conflitos de nomes.

/*********************************
 *      INCOMPLETE TYPES         *
 **********************************/
// Lista completa de todos os tipos exportados pela lib,
// só pra poder utilizar pointers de uns para os outros na definição dos tipos.

struct GLutil_Color;
struct GLutil_GLColor;

struct  GLutil_Vec2i;
struct  GLutil_Vec2f;
struct  GLutil_Vec3f;
struct  GLutil_Vec4f;
union   GLutil_Mat2x2f;
struct  GLutil_Mat3x3f;
struct  GLutil_Mat4x4f;

struct GLutil_3DBasis;
struct GLutil_Camera;

enum   GLutil_ShaderType;
struct GLutil_ShaderProgram;

struct GLutil_Vertex;
struct GLutil_Vertices;
struct GLutil_Faces;
struct GLutil_ObjData;
struct GLutil_Model;

enum   X11_Codes;
struct GLutil_MouseMove;
struct GLutil_InputState;
struct GLutil_Input;

struct GLutil_PointLight;
struct GLutil_PointLights;

struct GLutil;


/**********************************
 *          CORES                 *
 **********************************/

// Definição de cor compátivel com a definição de outras libs, por exemplo, do raylib.  
// O que é útil, pq dá pra integrar com essas libs, mas também, pq é possível 
// CHUPINHAR o código deles heheheheh (checar licencas no final do arquivo)   
typedef struct GLutil_Color {
    // 0 <= red, green, blue, alpha <= 255
    unsigned char red;  
    unsigned char green;
    unsigned char blue; 
    unsigned char alpha;
} GLutil_Color; 
                

// Definição de cor compátivel com a utilizada no GLSL 
typedef struct GLutil_GLColor {
//  0.0f <= r,g,b <= 1.0f.  
    float r;
    float g;
    float b;
    // @NOTE: adicionar alpha aqui?
} GLutil_GLColor;


// .
// .
// .
// .


/**********************************
 * MATEMÁTICA | ALGEBRA LINEAR    *
 **********************************/

// vetores
typedef struct GLutil_Vec2i {
    int x, y;
} GLutil_Vec2i;

typedef struct  GLutil_Vec2f {
    float x, y;
} GLutil_Vec2f;

typedef struct GLutil_Vec3f {
    float x, y, z;
} GLutil_Vec3f;

typedef struct GLutil_Vec4f {
    float x, y, z, w;
} GLutil_Vec4f;

// matrizes
typedef union GLutil_Mat2x2f {
    float data[4];

    GLutil_Vec2f rows[2];

    struct {
        // _[row][col]
        float _11; float _12;
        float _21; float _22;
    };

} GLutil_Mat2x2f;

typedef GLutil_Mat2x2f GLutil_Mat2f;

typedef struct GLutil_Mat3x3f {
    union {
        float data[9];

        GLutil_Vec3f rows[3];

        struct {
            float _11; float _12; float _13;
            float _21; float _22; float _23;
            float _31; float _32; float _33;
        };
    };


} GLutil_Mat3x3f;

typedef GLutil_Mat3x3f GLutil_Mat3f;

typedef struct GLutil_Mat4x4f {
    union {
        float data[16];

        GLutil_Vec4f rows[4];

        struct {
            float _11; float _12; float _13; float _14;
            float _21; float _22; float _23; float _24;
            float _31; float _32; float _33; float _34;
            float _41; float _42; float _43; float _44;
        };  
    };
} GLutil_Mat4x4f;

typedef GLutil_Mat4x4f GLutil_Mat4f;


// quaternions
typedef GLutil_Vec4f GLutil_Quaternion;


// .
// .
// .
// .


/*************************
 *       CAMERA          *
 *************************/

typedef struct GLutil_3DBasis {
    GLutil_Vec3f front, right, up; 
} GLutil_3DBasis;

// @NOTE: Isso aqui deveria receber só a struct GLutil_Camera? 
//        Soa mais razoável do que passar o estado da aplicação inteira.
//        A única coisa que realmente precisa da struct GLutil é o input,
//        então talvez mudar para algo como:
//        void (*GLutil_camera_update_func)(Camera* cam, Input input); 
// @TODO: Adicionar float dt como parâmetro, para poder atualizar de
//        maneira independente do frame rate.
typedef void (*GLutil_camera_update_func)(struct GLutil* glutil); 

typedef struct GLutil_Camera {
    GLutil_Vec3f pos;
    GLutil_Vec3f target;
    GLutil_Vec3f up;

    float fovX;
    float near;
    float far;

    GLutil_camera_update_func update;
} GLutil_Camera;


// .
// .
// .
// .


/********************************
*      SHADERS && 3D MODELS     *
********************************/

typedef enum GLutil_ShaderType {
    GLUTIL_PROGRAM_SHADER,
    GLUTIL_VERT_SHADER = GL_VERTEX_SHADER,
    GLUTIL_FRAG_SHADER = GL_FRAGMENT_SHADER,
} GLutil_ShaderType;

typedef struct GLutil_ShaderProgram {
    GLuint id;

    // Útil para reportar erros de compilação.
    const char* vert_shader_filename;
    const char* frag_shader_filename;
} GLutil_ShaderProgram;


typedef struct GLutil_Vertex {
    GLutil_Vec3f pos;

    GLutil_GLColor color;

    GLutil_Vec2f tex_pos;

    GLutil_Vec3f normal;
} GLutil_Vertex;


typedef struct GLutil_Vertices {
    GLutil_Vertex* items;
    size_t         len;
    size_t         cap;
} GLutil_Vertices;

typedef struct GLutil_Faces {
    unsigned int* items;
    size_t        len;
    size_t        cap;
} GLutil_Faces;



typedef struct GLutil_ObjData {
    GLutil_Vertices vertices;  // vertex data parsed from .obj file
    GLutil_Faces    faces;     // face   data parsed from .obj file

    GLuint vbo_id, ebo_id, vao_id;  // ids (or names?) returned by openGL when vertex data is loaded into the GPU

                                    
} GLutil_ObjData;

typedef struct GLutil_Model {
    float             scale;
    GLutil_Vec3f      world_pos;
    GLutil_Quaternion orientation;

    GLutil_ObjData*   geometry;
    GLuint            texture_id;  // id to the texture_id returned by OpenGL when the texture data is loaded into the GPU
                                   // a value of -1 (4294967295), means there's no texture associated with this model 
} GLutil_Model;


// .
// .
// .
// .


/*************************
 *       INPUT           *
 *************************/

typedef enum {
    X11_MOUSE_BUTTON_RIGHT     = 1, 
    X11_MOUSE_WHEEL_CLICK      = 2, 
    X11_MOUSE_BUTTON_LEFT      = 3, 
    X11_MOUSE_WHEEL_UP         = 4, 
    X11_MOUSE_WHEEL_DOWN       = 5, 
    X11_MOUSE_SIDE_BUTTON_UP   = 8, 
    X11_MOUSE_SIDE_BUTTON_DOWN = 9, 
} X11_MouseButtons;


// /usr/share/X11/xkb/keycodes
typedef enum {
  	X11_NUM_1                  =  10,  // <AE01>
	X11_NUM_2                  =  11,  // <AE02>
	X11_NUM_3                  =  12,  // <AE03>
	X11_NUM_4                  =  13,  // <AE04>
	X11_NUM_5                  =  14,  // <AE05>
	X11_NUM_6                  =  15,  // <AE06>
	X11_NUM_7                  =  16,  // <AE07>
	X11_NUM_8                  =  17,  // <AE08>
	X11_NUM_9                  =  18,  // <AE09>
	X11_NUM_0                  =  19,  // <AE10>
    X11_MINUS                  =  20,  // <AE11> 
	X11_EQUALS                 =  21,  // <AE12>
    X11_BACKSPACE              =  22,  // <BKSP>
    
    X11_TAB                    =  23,  // <TAB>
	X11_Q                      =  24,  // <AD01>
	X11_W                      =  25,  // <AD02>
	X11_E                      =  26,  // <AD03>
	X11_R                      =  27,  // <AD04>
	X11_T                      =  28,  // <AD05>
	X11_Y                      =  29,  // <AD06>
	X11_U                      =  30,  // <AD07>
	X11_I                      =  31,  // <AD08>
	X11_O                      =  32,  // <AD09>
	X11_P                      =  33,  // <AD10>
	X11_LEFT_BRACKET           =  34,  // <AD11>
	X11_RIGHT_BRACKET          =  35,  // <AD12>
    X11_BACKSLASH              =  51,  // <BKSL>
    X11_RETURN                 =  36,  // <RTRN>

    X11_CAPS                   =  66,  // <CAPS>
	X11_A                      =  38,  // <AC01>
	X11_S                      =  39,  // <AC02> 
	X11_D                      =  40,  // <AC03> 
	X11_F                      =  41,  // <AC04> 
	X11_G                      =  42,  // <AC05> 
	X11_H                      =  43,  // <AC06> 
	X11_J                      =  44,  // <AC07> 
	X11_K                      =  45,  // <AC08> 
	X11_L                      =  46,  // <AC09> 
    X11_SEMICOLON              =  47,  // <AC10>
    X11_SINGLE_QUOTE           =  48,  // <AC11>
    X11_TILDE                  =  49,  // <TLDE>

 	X11_LEFT_SHIFT             =  50,  // <LFSH> 
	X11_LESS_GREATER_THAN      =  94,  // <LSGT> , @NOTE: layout europeu tem uma única tecla para menor E maior que?
	X11_Z                      =  52,  // <AB01>
	X11_X                      =  53,  // <AB02>
	X11_C                      =  54,  // <AB03>
	X11_V                      =  55,  // <AB04>
	X11_B                      =  56,  // <AB05>
	X11_N                      =  57,  // <AB06>
	X11_M                      =  58,  // <AB07>
	X11_COMMA                  =  59,  // <AB08>
	X11_PERIOD                 =  60,  // <AB09>
	X11_FORWARD_SLASH          =  61,  // <AB10>
	X11_RIGHT_SHIFT            =  62,  // <RTSH>

    X11_LEFT_CONTROL           =  37,  // <LCTL>
	X11_LEFT_SUPER             = 133,  // <LWIN>
	X11_LEFT_ALT               =  64,  // <LALT>
	X11_SPACE                  =  65,  // <SPCE>
	X11_RIGHT_ALT              = 108,  // <RALT>
	X11_RIGHT_SUPER            = 134,  // <RWIN>
	X11_MENU_COMPOSE           = 135,  // <COMP>
	X11_RIGHT_CONTROL          = 105,  // <RCTL>

    X11_ESC                    =   9,  // <ESC>
	X11_F1                     =  67,  // <FK01>
	X11_F2                     =  68,  // <FK02>
	X11_F3                     =  69,  // <FK03>
	X11_F4                     =  70,  // <FK04>
	X11_F5                     =  71,  // <FK05>
	X11_F6                     =  72,  // <FK06>
	X11_F7                     =  73,  // <FK07>
	X11_F8                     =  74,  // <FK08>
	X11_F9                     =  75,  // <FK09>
	X11_F10                    =  76,  // <FK10>
	X11_F11                    =  95,  // <FK11>
	X11_F12                    =  96,  // <FK12>

    X11_PRINT_SCREEN           = 107, // <PRSC>
	X11_SCROLL_LOCK            =  78, // <SCLK>
	X11_PAUSE                  = 127, // <PAUS>

    X11_INSERT                 = 118, // <INS>
	X11_HOME                   = 110, // <HOME>
	X11_PAGE_UP                = 112, // <PGUP>

	X11_DELETE                 = 119, // <DELE>
	X11_END                    = 115, // <END>
	X11_PAGE_DOWN              = 117, // <PGDN>

	X11_ARROW_UP               = 111, // <UP>
	X11_ARROW_LEFT             = 113, // <LEFT>
	X11_ARROW_DOWN             = 116, // <DOWN>
	X11_ARROW_RIGHT            = 114, // <RGHT>
	X11_NUMLOCK                =  77, // <NMLK>
	X11_KEYPAD_DIVIDE          = 106, // <KPDV>
	X11_KEYPAD_MULTIPLY        =  63, // <KPMU>
	X11_KEYPAD_SUBTRACT        =  82, // <KPSU>
    X11_KEYPAD_7               =  79, // <KP7>
    X11_KEYPAD_8               =  80, // <KP8>
    X11_KEYPAD_9               =  81, // <KP9>
    X11_KEYPAD_ADD             =  86, // <KPAD>
	X11_KEYPAD_4               =  83, // <KP4>
	X11_KEYPAD_5               =  84, // <KP5>
	X11_KEYPAD_6               =  85, // <KP6>
	X11_KEYPAD_1               =  87, // <KP1>
	X11_KEYPAD_2               =  88, // <KP2>
	X11_KEYPAD_3               =  89, // <KP3>
	X11_KEYPAD_ENTER           = 104, // <KPEN>
	X11_KEYPAD_0               =  90, // <KP0>
	X11_KEYPAD_DELETE          =  91, // <KPDL>
	X11_KEYPAD_EQUALS          = 125, // <KPEQ>
	X11_F13                    = 191, // <FK13>
	X11_F14                    = 192, // <FK14>
	X11_F15                    = 193, // <FK15>
	X11_F16                    = 194, // <FK16>
	X11_F17                    = 195, // <FK17>
	X11_F18                    = 196, // <FK18>
	X11_F19                    = 197, // <FK19>
	X11_F20                    = 198, // <FK20>
	X11_F21                    = 199, // <FK21>
	X11_F22                    = 200, // <FK22>
	X11_F23                    = 201, // <FK23>
	X11_F24                    = 202, // <FK24>

} X11_KeyCodes;


//    X11 KEYCODE              STRUCT FIELD NAME
#define X11_KEYBOARD_EVENTS \
    X(X11_NUM_1              , num_1               ) \
    X(X11_NUM_2              , num_2               ) \
    X(X11_NUM_3              , num_3               ) \
    X(X11_NUM_4              , num_4               ) \
    X(X11_NUM_5              , num_5               ) \
    X(X11_NUM_6              , num_6               ) \
    X(X11_NUM_7              , num_7               ) \
    X(X11_NUM_8              , num_8               ) \
    X(X11_NUM_9              , num_9               ) \
    X(X11_NUM_0              , num_0               ) \
    X(X11_MINUS              , minus               ) \
    X(X11_EQUALS             , equals              ) \
    X(X11_BACKSPACE          , backspace           ) \
    X(X11_TAB                , tab                 ) \
    X(X11_Q                  , q                   ) \
    X(X11_W                  , w                   ) \
    X(X11_E                  , e                   ) \
    X(X11_R                  , r                   ) \
    X(X11_T                  , t                   ) \
    X(X11_Y                  , y                   ) \
    X(X11_U                  , u                   ) \
    X(X11_I                  , i                   ) \
    X(X11_O                  , o                   ) \
    X(X11_P                  , p                   ) \
    X(X11_LEFT_BRACKET       , left_bracket        ) \
    X(X11_RIGHT_BRACKET      , right_bracket       ) \
    X(X11_BACKSLASH          , backslash           ) \
    X(X11_RETURN             , key_return          ) \
    X(X11_CAPS               , caps                ) \
    X(X11_A                  , a                   ) \
    X(X11_S                  , s                   ) \
    X(X11_D                  , d                   ) \
    X(X11_F                  , f                   ) \
    X(X11_G                  , g                   ) \
    X(X11_H                  , h                   ) \
    X(X11_J                  , j                   ) \
    X(X11_K                  , k                   ) \
    X(X11_L                  , l                   ) \
    X(X11_SEMICOLON          , semicolon           ) \
    X(X11_SINGLE_QUOTE       , single_quote        ) \
    X(X11_TILDE              , tilde               ) \
    X(X11_LEFT_SHIFT         , left_shift          ) \
    X(X11_LESS_GREATER_THAN  , less_greater_than   ) \
    X(X11_Z                  , z                   ) \
    X(X11_X                  , x                   ) \
    X(X11_C                  , c                   ) \
    X(X11_V                  , v                   ) \
    X(X11_B                  , b                   ) \
    X(X11_N                  , n                   ) \
    X(X11_M                  , m                   ) \
    X(X11_COMMA              , comma               ) \
    X(X11_PERIOD             , period              ) \
    X(X11_FORWARD_SLASH      , forward_slash       ) \
    X(X11_RIGHT_SHIFT        , right_shift         ) \
    X(X11_LEFT_CONTROL       , left_control        ) \
    X(X11_LEFT_SUPER         , left_super          ) \
    X(X11_LEFT_ALT           , left_alt            ) \
    X(X11_SPACE              , space_bar           ) \
    X(X11_RIGHT_ALT          , right_alt           ) \
    X(X11_RIGHT_SUPER        , right_super         ) \
    X(X11_MENU_COMPOSE       , menu_compose        ) \
    X(X11_RIGHT_CONTROL      , right_control       ) \
    X(X11_ESC                , esc                 ) \
    X(X11_F1                 , f1                  ) \
    X(X11_F2                 , f2                  ) \
    X(X11_F3                 , f3                  ) \
    X(X11_F4                 , f4                  ) \
    X(X11_F5                 , f5                  ) \
    X(X11_F6                 , f6                  ) \
    X(X11_F7                 , f7                  ) \
    X(X11_F8                 , f8                  ) \
    X(X11_F9                 , f9                  ) \
    X(X11_F10                , f10                 ) \
    X(X11_F11                , f11                 ) \
    X(X11_F12                , f12                 ) \
    X(X11_PRINT_SCREEN       , print_screen        ) \
    X(X11_SCROLL_LOCK        , scroll_lock         ) \
    X(X11_PAUSE              , pause               ) \
    X(X11_INSERT             , insert              ) \
    X(X11_HOME               , home                ) \
    X(X11_PAGE_UP            , page_up             ) \
    X(X11_DELETE             , delete              ) \
    X(X11_END                , end                 ) \
    X(X11_PAGE_DOWN          , page_down           ) \
    X(X11_ARROW_UP           , arrow_up            ) \
    X(X11_ARROW_LEFT         , arrow_left          ) \
    X(X11_ARROW_DOWN         , arrow_down          ) \
    X(X11_ARROW_RIGHT        , arrow_right         ) \
    X(X11_NUMLOCK            , numlock             ) \
    X(X11_KEYPAD_DIVIDE      , keypad_divide       ) \
    X(X11_KEYPAD_MULTIPLY    , keypad_multiply     ) \
    X(X11_KEYPAD_SUBTRACT    , keypad_subtract     ) \
    X(X11_KEYPAD_7           , keypad_7            ) \
    X(X11_KEYPAD_8           , keypad_8            ) \
    X(X11_KEYPAD_9           , keypad_9            ) \
    X(X11_KEYPAD_ADD         , keypad_add          ) \
    X(X11_KEYPAD_4           , keypad_4            ) \
    X(X11_KEYPAD_5           , keypad_5            ) \
    X(X11_KEYPAD_6           , keypad_6            ) \
    X(X11_KEYPAD_1           , keypad_1            ) \
    X(X11_KEYPAD_2           , keypad_2            ) \
    X(X11_KEYPAD_3           , keypad_3            ) \
    X(X11_KEYPAD_ENTER       , keypad_enter        ) \
    X(X11_KEYPAD_0           , keypad_0            ) \
    X(X11_KEYPAD_DELETE      , keypad_delete       ) \
    X(X11_KEYPAD_EQUALS      , keypad_equals       ) \
    X(X11_F13                , f13                 ) \
    X(X11_F14                , f14                 ) \
    X(X11_F15                , f15                 ) \
    X(X11_F16                , f16                 ) \
    X(X11_F17                , f17                 ) \
    X(X11_F18                , f18                 ) \
    X(X11_F19                , f19                 ) \
    X(X11_F20                , f20                 ) \
    X(X11_F21                , f21                 ) \
    X(X11_F22                , f22                 ) \
    X(X11_F23                , f23                 ) \
    X(X11_F24                , f24                 ) 



typedef struct GLutil_MouseMove {
    bool         moved;
    GLutil_Vec2i last_pos;
    GLutil_Vec2i pos;
} GLutil_MouseMove;

typedef struct GLutil_InputState {
    bool down:    1; // 'down'    significa que no frame anterior esse botão/tecla não estava apertado e agora está
    bool up:      1; // 'up'      significa que no frame anterior esse botão/tecla estava apertado e agora não está mais
    bool pressed: 1; // 'pressed' significa que esse botão/tecla está sendo apertado
} GLutil_InputState;


// @TODO: talvez refatorar input para um XMacro?
typedef struct GLutil_Input {
    GLutil_InputState mouse_button_right;
    GLutil_InputState mouse_button_left;
    GLutil_MouseMove  mouse_movement;
    
#define X(_, key) GLutil_InputState key;
    X11_KEYBOARD_EVENTS
#undef X

} GLutil_Input;

// .
// .
// .
// .


/*************************
 *       LIGHTING        *
 *************************/

// Uma fonte pontual de luz. Usada no fragment shader.
typedef struct GLutil_PointLight {
    float          intensity;
    float          shine;
    GLutil_Vec3f   pos;
    GLutil_GLColor clr;

    GLuint         shadow_map;    // id da texture
    GLuint         shadow_buffer; // id do framebuffer

    GLutil_Camera  pov;           // perspectiva do PointLight, mirando no target de glutil->camera.
    GLutil_Mat4x4f space_matrix;  // projection * view da perspectiva da luz.
} GLutil_PointLight;

 // Array Dinâmico para armazenar todas as fontes pontuais de luz 
typedef struct GLutil_PointLights {
    GLutil_PointLight* items;
    size_t             len;
    size_t             cap;
} GLutil_PointLights; 


// .
// .
// .
// .


/*******************************************************************************
 *                        THE BIG FAT GLUTIL                                   *
 *******************************************************************************/
typedef struct GLutil {

/** X11 STUFF **************/
    Window       window;
    Display*     display;
    GLXContext   ctx;
    XVisualInfo* vi;
    GLXFBConfig  fbc;
/** END OF X11 STUFF *******/

    int winwidth;
    int winheight;

    uint64_t     prev_t_nanos;    // timestamp em nanosegundos do último frame
    uint64_t     cur_t_nanos;     // timestamp em nanosegundos do frame atual
    uint64_t     dt_nanos;        // delta time em nanosegundos entre os dois últimos frames
    uint64_t     target_sleep_us; // quantidade de microsegundos de delay para atingir o FPS alvo

    GLuint       fullscreen_vao ; // um vao de quad básico que ocupa a tela inteira,
                                  // usado pra desenhar texture 

    GLutil_Camera camera;
    GLutil_Input  input;

    GLutil_PointLights lights;

    GLutil_ShaderProgram* active_shader;          

    // @NOTE: pointer de static memory dentro das funções de mesmo nome
    GLutil_ShaderProgram* render_shader;        // o main renderer
    GLutil_ShaderProgram* shadow_shader;        // computa o shadow map
    GLutil_ShaderProgram* compatibility_shader; // renderer compátivel com OpenGL 1.1
    GLutil_ShaderProgram* texture_shader;       // desenha uma texture diretamente na tela
} GLutil;




///////////////////////////////////////////////////////////////////////////////


 

/*******************************************************************************
 *                                                                             *
 *                    FUNCTION FORWARD DECLARATIONS                            *
 *                                                                             *
 ******************************************************************************/
// Todos as funções aqui declaradas com o prefixo GLutil_ podem ser acessadas sem
// esse prefixo. O prefixo é incluído como um mecanismo que permita contornar 
// situações de conflitos de nomes.

void GLutil_init(GLutil* glutil, unsigned int w, unsigned int h);              // inicializa Display, Window e o OpenGLContext. e mais alguns estados internos
void GLutil_destroy(GLutil* glutil);                                           // desaloca e fecha todos os recursos alocados por GLutil.h

void GLutil_hide_cursor(GLutil* glutil);


// FRAME LOOP
uint64_t GLutil_get_time_nanos(void);                                          // retorna o número de nanosegundos desde o UNIX epoch
float    GLutil_get_time_secs(GLutil* glutil);                                 // retorna o número de segundos dedsde o UNIX epoch. @NOTE: esse função calcula o número de segundos com base no valor de nanosegundos armazenado na struct GLutil, que é atualizada com base no delta dos fraames, ao invés de fazer outra systemcall .
uint64_t GLutil_frame_delta_nanos(GLutil *glutil);                             // quantidade de nanosegundos que se passou entre o último frame e o frame atual
uint64_t GLutil_begin_frame(GLutil *glutil);                                   // deve ser chamada no ínicio de toda iteração do FRAME LOOP
void     GLutil_clear_bg( GLutil_Color color );                                // colore a janela inteira com uma cor sólida
void     GLutil_end_frame(GLutil *glutil);                                     // faz o swap dos buffers E atualiza o estado interno do GLutil (principalmente o delta entre frames)
void     GLutil_present(GLutil* glutil);                                       // faz o swap dos buffers do opengl
void     GLutil_set_target_fps(GLutil* glutil, float fps);                     // usa fps para calcular o delay entre frames como 1/fps e espera o delay somente se delay > 0
void     GLutil_unlimited_fps(GLutil* glutil);                                 // coloca zero nanos de delay entre frames
void     GLutil_update_input(GLutil* glutil);                                  // checa a file de eventos do X11 pra atualizar o estado do input armazenado na struct GLutil
bool     GLutil_next_event(GLutil* glutil, XEvent* event);                     // consome o próximo evento na file de eventos do X11
void     GLutil_check_gl_error_(int line, const char* file, const char* func); // @NOTE: essa função não foi feita pra ser usada diretamente, e sim por meio do macro GL_CHECK_ERRORS


// api legado do OpenGL (immediate mode)
void GLutil_circle(GLutil_Vec2f center, int radius, GLutil_Color color);       // desenha um círculo através de GL_TRIANGLE_FAN, o nível de precisão do círculo pode ser alterado pelo macro GLUTIL_CIRCLE_PRECISION
void GLutil_line(GLutil_Vec2f a, GLutil_Vec2f b, GLutil_Color clr, float w);
void GLutil_rect(GLutil_Vec2f topleft, float w, float h, GLutil_Color clr);  
void GLutil_ortho2D(GLdouble left, GLdouble right, GLdouble top, GLdouble bottom);

// vetores
GLutil_Vec2f GLutil_Vec2f_add(GLutil_Vec2f a, GLutil_Vec2f b);
GLutil_Vec2f GLutil_Vec2f_sub(GLutil_Vec2f a, GLutil_Vec2f b);
float GLutil_Vec2f_sqlength(GLutil_Vec2f v);
float GLutil_Vec2f_length(GLutil_Vec2f v);
GLutil_Vec2f GLutil_Vec2f_rotccw(GLutil_Vec2f v, float theta);
GLutil_Vec2f GLutil_Vec2f_rotcw(GLutil_Vec2f v, float theta);
float GLutil_Vec2f_dot(GLutil_Vec2f a, GLutil_Vec2f b);
GLutil_Vec2f GLutil_Vec2f_apply_mat2x2(GLutil_Vec2f v, GLutil_Mat2x2f A);
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
float GLutil_Vec3f_angle_between(GLutil_Vec3f a, GLutil_Vec3f b);
GLutil_Vec3f GLutil_Vec3f_apply_mat3x3(GLutil_Vec3f v, GLutil_Mat3x3f A);
GLutil_Vec3f GLutil_Vec3f_rotate_x(GLutil_Vec3f v, float angle);
GLutil_Vec3f GLutil_Vec3f_rotate_y(GLutil_Vec3f v, float angle);
GLutil_Vec3f GLutil_Vec3f_rotate_z(GLutil_Vec3f v, float angle);
GLutil_Vec3f GLutil_Vec3f_rotate_around_axis(GLutil_Vec3f v, GLutil_Vec3f R, float angle); // rotação usando multiplicação de matrizes
GLutil_Vec3f GLutil_Vec3f_rotate(GLutil_Vec3f v, float angle, GLutil_Vec3f axis);          // rotação usando quaternions

GLutil_Vec4f GLutil_Vec4f_apply_mat4x4(GLutil_Vec4f v, GLutil_Mat4x4f A);
float GLutil_Vec4f_dot(GLutil_Vec4f a, GLutil_Vec4f b);

// matrizes
GLutil_Mat2x2f GLutil_Mat2x2f_mul(GLutil_Mat2x2f A, GLutil_Mat2x2f B);
GLutil_Mat3x3f GLutil_Mat3x3f_mul(GLutil_Mat3x3f A, GLutil_Mat3x3f B);
GLutil_Mat4x4f GLutil_Mat4x4f_mul(GLutil_Mat4x4f a, GLutil_Mat4x4f b);
GLutil_Mat4x4f GLutil_Mat4x4f_transpose(GLutil_Mat4x4f A);

// quaternions
GLutil_Quaternion GLutil_unit_quaternion(); // { 1.0f, 0.0f, 0.0f, 0.0f } == quaternion "padrão" que representa ausência de rotação alguma
GLutil_Quaternion GLutil_quat_from_angle_axis(float angle, GLutil_Vec3f axis);
GLutil_Quaternion GLutil_quat_from_vec3f(GLutil_Vec3f v);
int               GLutil_quat_sqlength(GLutil_Quaternion q);
GLutil_Quaternion GLutil_quat_conjugate(GLutil_Quaternion q);
GLutil_Quaternion GLutil_quat_scale(GLutil_Quaternion q, float s);
GLutil_Quaternion GLutil_quat_div_scalar(GLutil_Quaternion v, float d);
GLutil_Quaternion GLutil_quat_mul(GLutil_Quaternion a, GLutil_Quaternion b);
GLutil_Quaternion GLutil_quat_inverse(GLutil_Quaternion q);


// CAMERA
GLutil_Mat3x3f GLutil_model_matrix(GLutil_Camera cam);
GLutil_Mat4x4f GLutil_view_matrix(GLutil_Camera cam);
GLutil_Mat4x4f GLutil_projection_matrix(GLutil_Camera cam, float aspectRatio); 
// @TODO: pensar sobre onde armazenar as informações de width e height da janela, 
//        de maneira a opcionalmente não precisar passar o argumento aspectRatio 
// @NOTE: talvez salvar aspectRatio na própria Camera?

GLutil_Vec3f GLutil_camera_direction(GLutil_Camera* camera);
GLutil_Vec3f GLutil_camera_right(GLutil_Camera* camera);
GLutil_Vec3f GLutil_camera_up(GLutil_Camera* camera);
GLutil_3DBasis GLutil_camera_basis(GLutil_Camera* camera);


void GLutil_camera_update(GLutil* glutil);

// atualiza a posição da câmera para avançar um passo na direção da tecla pressionada
void GLutil_camera_update_step(GLutil* glutil, float step_size);

// orbiter camera: mantém sempre o mesmo target, mas muda a posição da câmera pra orbitar ao redor do target
void GLutil_camera_orbiter(GLutil* glutil);
void GLutil_orbiter_camera_update(GLutil* glutil);
GLutil_Camera GLutil_camera_orbit(GLutil_Camera camera, float theta, float phi);

// first person camera: mantém sempre a mesma posição, mas muda o target pra apontar pra onde o mouse aponta
void GLutil_camera_first_person(GLutil* glutil);
void GLutil_first_person_camera_update(GLutil* glutil);

// SHADERS
GLutil_ShaderProgram GLutil_shader_new(const char* vert_shader, const char* frag_shader); // Cria um ShaderProgram através da compilação e linking do arquivos de shader nos parâmetros

GLutil_ShaderProgram* GLutil_render_shaders();         // Ativa o shader padrão pra rasterizar cenas
GLutil_ShaderProgram* GLutil_shadow_shaders();         // Ativa o shader para gerar o shadow_map
GLutil_ShaderProgram* GLutil_compatibility_shaders();  // Ativa o shader de compatibilidade para permitir o uso das funções de modo imediato
GLutil_ShaderProgram* GLutil_texture_shaders();        // Ativa o shader de texturas para desenhar texturas em posições arbitrárias

void GLutil_shader_reload(GLutil_ShaderProgram* shader);  // Delete o ShaderProgram associado a shader->id (se existe) e recompila e relinka os arquivos fontes.
GLuint GLutil_shader_load_vert(const char* name); // Wrapper de shader_load
GLuint GLutil_shader_load_frag(const char* name); // Wrapper de shader_load
GLuint GLutil_shader_load(const char* name, GLutil_ShaderType type); // Lê o código fonte de um arquivo de vertex ou fragment shader, compila ele e retorna o id do shader compilado. Majoritariamente feito pra uso interno da lib, mas pode ser útil.
void GLutil_check_shader_compilation_error_(GLuint shader_id, const char* shader_file, GLutil_ShaderType type, int line, const char* file, const char* func); // não usar diretamente. função feita pra ser usada através do macro GLutil_check_shader_compilation_error

void GLutil_shader_use(GLutil* glutil, GLutil_ShaderProgram* shader); // marca o id do shader como ativo no opengl e salva a ShaderProgram em glutil->active_shader

// funções auxialiares pra passar uniforms para os shaders
void GLutil_shader_add_int(GLutil_ShaderProgram shader, const char* name, int value);
void GLutil_shader_add_float(GLutil_ShaderProgram shader, const char* name, float value);
void GLutil_shader_add_vec2f(GLutil_ShaderProgram shader, const char* name, GLutil_Vec2f value);
void GLutil_shader_add_vec3f(GLutil_ShaderProgram shader, const char* name, GLutil_Vec3f value);
void GLutil_shader_add_glcolor(GLutil_ShaderProgram shader, const char* name, GLutil_GLColor value);
void GLutil_shader_add_vec4f(GLutil_ShaderProgram shader, const char* name, GLutil_Vec4f value);
void GLutil_shader_add_quaternion(GLutil_ShaderProgram shader, const char* name, GLutil_Quaternion value);
void GLutil_shader_add_mat3f(GLutil_ShaderProgram shader, const char* name, GLutil_Mat3x3f value);
void GLutil_shader_add_mat4f(GLutil_ShaderProgram shader, const char* name, GLutil_Mat4x4f value);
void GLutil_shader_add_mat4f_transposed(GLutil_ShaderProgram shader, const char* name, GLutil_Mat4x4f value);

// funçõezinhas de compatibilidade com modo imediato
void GLutil_immediate_mode_begin(GLutil* glutil);
void GLutil_immediate_mode_end(GLutil* glutil);


// MODELS, OBJ 
GLutil_ObjData GLutil_load_obj_file(const char* file_path);
void GLutil_load_obj_to_GPU(GLutil_ObjData* obj);

GLutil_Model GLutil_make_model();
GLutil_Model GLutil_model_from_obj(GLutil_ObjData* obj);
void GLutil_render_model(GLutil* glutil, GLutil_Model* model);

// TEXTURES 
void GLutil_setup_fullscreen_buffer(GLutil* g);
GLuint GLutil_generate_texture_2D();
GLuint GLutil_texture_from_pixels(int img_width, int img_height, unsigned char* pixels);
GLuint GLutil_texture_from_pixels_opt(int img_width, int img_height, unsigned char* pixels, GLenum texture_pixel_fmt, GLenum data_pixel_fmt);
GLuint GLutil_texture_from_image(const char* image_file_name);
void GLutil_render_texture(GLutil* g, GLuint texture, int unit, float x, float y, float w, float h);


// LIGHTS & SHADOWS
int GLutil_add_light(GLutil* glutil, GLutil_PointLight light);
GLutil_PointLight* GLutil_get_light(GLutil* glutil, int light_id);
void GLutil_generate_shadow_map_texture(GLutil_PointLight* l);
void GLutil_activate_shadow_buffer(GLutil* g, GLutil_PointLight* l);
void GLutil_deactivate_shadow_buffer(GLutil* g, GLutil_PointLight* l, GLutil_Camera old_cam);
GLutil_Mat4x4f GLutil_light_space_matrix(GLutil_PointLight l);

void GLutil_compute_shadow_maps(GLutil* g, int models_count, GLutil_Model** models);

// OTHER STUFF?
void GLutil_wireframe_mode();

void GLutil_print_framebuffer_status(GLenum status);


///////////////////////////////////////////////////////////////////////////////

 
 

/*******************************************************************************
 *                                                                             *
 *                        MACROS ÚTEIS                                         *
 *                                                                             *
 ******************************************************************************/

#define GLUTIL_RAD2DEG(angle) ((angle) * 180.0f / M_PI)
#define GLUTIL_MAP(x, src_min, src_max, dst_min, dst_max) ( (dst_min) + ( ( ( (x) - (src_min) ) * ( (dst_max) - (dst_min) ) ) / ( (src_max) - (src_min) ) ))
#define GLUTIL_RAND(min, max) MAP((float)rand(), 0.0f, (float)RAND_MAX, (float)(min), (float)(max))

#define GLUTIL_COLOR(r, g, b) (Color){ .red = (r), .green = (g), .blue = (b), .alpha = 255 }
#define GLUTIL_BLACK COLOR(0, 0, 0)

#define GLUTIL_PRINT_ERROR(msg) fprintf(stderr, "%d:%s:%s ERROR: %s", __LINE__, __FILE__, __FUNCTION__, msg)
#define GLUTIL_CHECK_GL_ERROR() GLutil_check_gl_error_(__LINE__, __FILE__, __FUNCTION__)

#define GLutil_check_shader_compilation_error(shader_id, shader_file, type) GLutil_check_shader_compilation_error_(shader_id, shader_file, type, __LINE__, __FILE__, __FUNCTION__);

/*
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
*/

// @TODO: organizar melhor esseas flags de eventos
#define ALL_INPUT_MASKS ( \
        KeyPressMask       | KeyReleaseMask         | ButtonPressMask          \
      | ButtonReleaseMask  | EnterWindowMask        | LeaveWindowMask          \
      | PointerMotionMask  | Button1MotionMask        \
      | Button2MotionMask  | Button3MotionMask      | Button4MotionMask        \
      | Button5MotionMask  | ButtonMotionMask       | KeymapStateMask          \
      | ExposureMask       | VisibilityChangeMask   | StructureNotifyMask      \
      | ResizeRedirectMask | SubstructureNotifyMask | SubstructureRedirectMask \
      | FocusChangeMask    | PropertyChangeMask     | ColormapChangeMask       \
      | OwnerGrabButtonMask )
 



///////////////////////////////////////////////////////////////////////////////

 
 
 
/*******************************************************************************
 *                                                                             *
 *                        NO PREFIX DEFINITIONS                                *
 *                                                                             *
 ******************************************************************************/
// Todos os tipos e funções declaradas acima são repetidas aqui, porém sem o 
// prefixo GLutil_. Dessa forma é possível ter um controle minucioso de quais
// funções devem ser realmente acessadas com o prefixo, no caso de um conflito de
// nomes. 
//
// Por exemplo, suponha que no seu projeto já exista um tipo com o nome 'Camera'.
// Ao fazer o include do GLutil:
//      ```
//      #define GLUTIL_IMPLEMENTATION
//      #include "glutil.h"
//      ```
// você acabaria por incluir também a linha abaixo que define:
//      ```
//      #define GLutil_Camera Camera
//      ```
// o que geraria um conflito no nome Camera.
//
// Isso poderia ser contornado forçando o uso de prefixo em todos os tipos e todas
// as funções:
//
//      ```
//      #define GLUTIL_FORCE_PREFIX
//      #define GLUTIL_IMPLEMENTATION
//      #include "glutil.h"
//      ```
// mas essa não é uma solução ideal, pois forçaria o uso do prefixo em tipos e 
// funções que não estão envolvidas em nehuma forma de conflito. Não há motivos
// para forçar o uso de GLutil_Vec3f se não há conflito com Vec3f.
//
// Uma solução melhor, seria descartar a versão sem prefixo somente do elemento
// que realmente está envolvido no conflito, neste exemplo, Camera:
//      ```
//      #define GLUTIL_IMPLEMENTATION
//      #include "glutil.h"
//      #undef Camera
//      ```
// Dessa forma, não haveria conflito com esse nome específico, mas também seria
// possivel acessar todos os outros tipos e funções sem o prefixo GLutil.


#ifndef GLUTIL_FORCE_PREFIX

#define MAP    GLUTIL_MAP
#define RAND   GLUTIL_RAND
#define COLOR  GLUTIL_COLOR
#define BLACK  GLUTIL_BLACK

#define Color   GLutil_Color
#define GLColor GLutil_GLColor

#define ShaderType    GLutil_ShaderType
#define ShaderProgram GLutil_ShaderProgram

#define Vec2i   GLutil_Vec2i
#define Vec2f   GLutil_Vec2f
#define Vec3f   GLutil_Vec3f
#define Vec4f   GLutil_Vec4f

#define Mat2x2f  GLutil_Mat2x2f
#define Mat2f    GLutil_Mat2f
#define Mat3x3f  GLutil_Mat3x3f
#define Mat3f    GLutil_Mat3f
#define Mat4x4f  GLutil_Mat4x4f
#define Mat4f    GLutil_Mat4f

#define Quaternion GLutil_Quaternion

#define Camera    GLutil_Camera
#define _3DBasis  GLutil_3DBasis

#define Input      GLutil_Input
#define InputState GLutil_InputState
#define MouseMove  GLutil_MouseMove

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

#define Vec2i_add          GLutil_Vec2i_add
#define Vec2i_sub          GLutil_Vec2i_sub
#define Vec2i_sqlength     GLutil_Vec2i_sqlength
#define Vec2i_length       GLutil_Vec2i_length
#define Vec2i_rotccw       GLutil_Vec2i_rotccw
#define Vec2i_rotcw        GLutil_Vec2i_rotcw
#define Vec2i_dot          GLutil_Vec2i_dot
#define Vec2i_scale        GLutil_Vec2i_scale

#define Vec2f_add          GLutil_Vec2f_add
#define Vec2f_sub          GLutil_Vec2f_sub
#define Vec2f_sqlength     GLutil_Vec2f_sqlength
#define Vec2f_length       GLutil_Vec2f_length
#define Vec2f_rotccw       GLutil_Vec2f_rotccw
#define Vec2f_rotcw        GLutil_Vec2f_rotcw
#define Vec2f_dot          GLutil_Vec2f_dot
#define Vec2f_apply_mat2x2 GLutil_Vec2f_apply_mat2x2
#define Vec2f_scale        GLutil_Vec2f_scale

#define Vec3f_add                GLutil_Vec3f_add
#define Vec3f_sub                GLutil_Vec3f_sub
#define Vec3f_sqlength           GLutil_Vec3f_sqlength
#define Vec3f_length             GLutil_Vec3f_length
#define Vec3f_div                GLutil_Vec3f_div
#define Vec3f_normalize          GLutil_Vec3f_normalize
#define Vec3f_scale              GLutil_Vec3f_scale
#define Vec3f_dot                GLutil_Vec3f_dot
#define Vec3f_cross              GLutil_Vec3f_cross
#define Vec3f_angle_between      GLutil_Vec3f_angle_between
#define Vec3f_apply_mat3x3       GLutil_Vec3f_apply_mat3x3
#define Vec3f_rotate_x           GLutil_Vec3f_rotate_x
#define Vec3f_rotate_y           GLutil_Vec3f_rotate_y
#define Vec3f_rotate_z           GLutil_Vec3f_rotate_z
#define Vec3f_rotate_around_axis GLutil_Vec3f_rotate_around_axis // com matrizes 
#define Vec3f_rotate             GLutil_Vec3f_rotate             // com quaternions

#define Vec4f_dot          GLutil_Vec4f_dot
#define Vec4f_apply_mat4x4 GLutil_Vec4f_apply_mat4x4

#define Mat2x2f_mul       GLutil_Mat2x2f_mul
#define Mat3x3f_mul       GLutil_Mat3x3f_mul
#define Mat4x4f_mul       GLutil_Mat4x4f_mul
#define Mat4x4f_transpose GLutil_Mat4x4f_transpose

#define unit_quaternion      GLutil_unit_quaternion
#define quat_from_angle_axis GLutil_quat_from_angle_axis
#define quat_from_vec3f      GLutil_quat_from_vec3f
#define quat_sqlength        GLutil_quat_sqlength
#define quat_conjugate       GLutil_quat_conjugate
#define quat_scale           GLutil_quat_scale
#define quat_div_scalar      GLutil_quat_div_scalar
#define quat_mul             GLutil_quat_mul
#define quat_inverse         GLutil_quat_inverse


#define PointLight GLutil_PointLight
#define PointLights GLutil_PointLights


#define PRINT_ERROR     GLUTIL_PRINT_ERROR
#define CHECK_GL_ERROR  GLUTIL_CHECK_GL_ERROR
#define check_gl_error_ GLutil_check_gl_error_

#define get_time_nanos    GLutil_get_time_nanos
#define get_time_secs     GLutil_get_time_secs
#define begin_frame       GLutil_begin_frame
#define frame_delta_nanos GLutil_frame_delta_nanos
#define end_frame         GLutil_end_frame
#define present           GLutil_present
#define set_target_fps    GLutil_set_target_fps
#define unlimited_fps     GLutil_unlimited_fps
#define update_input      GLutil_update_input
#define next_event        GLutil_next_event
#define t_secs            GLutil_t_secs

#define render_shaders                 GLutil_render_shaders
#define shadow_shaders                 GLutil_shadow_shaders
#define compatibility_shaders          GLutil_compatibility_shaders
#define texture_shaders                GLutil_texture_shaders
#define check_shader_compilation_error GLutil_check_shader_compilation_error
#define shader_new                     GLutil_shader_new
#define shader_reload                  GLutil_shader_reload
#define shader_load                    GLutil_shader_load
#define shader_use                     GLutil_shader_use
#define shader_add_int                 GLutil_shader_add_int
#define shader_add_float               GLutil_shader_add_float
#define shader_add_vec2f               GLutil_shader_add_vec2f
#define shader_add_vec3f               GLutil_shader_add_vec3f
#define shader_add_glcolor             GLutil_shader_add_glcolor
#define shader_add_vec4f               GLutil_shader_add_vec4f
#define shader_add_quaternion          GLutil_shader_add_quaternion
#define shader_add_mat3f               GLutil_shader_add_mat3f
#define shader_add_mat4f               GLutil_shader_add_mat4f
#define shader_add_mat4f_transposed    GLutil_shader_add_mat4f_transposed
#define immediate_mode_begin           GLutil_immediate_mode_begin
#define immediate_mode_end             GLutil_immediate_mode_end

#define render_obj       GLutil_render_obj
#define load_obj_file    GLutil_load_obj_file
#define load_obj_to_GPU  GLutil_load_obj_to_GPU
#define model_from_obj   GLutil_model_from_obj
#define make_model       GLutil_make_model
#define render_model     GLutil_render_model

#define setup_fullscreen        GLutil_setup_fullscreen_buffer // why is this here?
#define generate_texture_2D     GLutil_generate_texture_2D
#define texture_from_pixels_opt GLutil_texture_from_pixels_opt
#define texture_from_pixels     GLutil_texture_from_pixels
#define texture_from_image      GLutil_texture_from_image
#define render_texture          GLutil_render_texture

#define model_matrix      GLutil_model_matrix
#define view_matrix       GLutil_view_matrix
#define projection_matrix GLutil_projection_matrix
#define camera_direction  GLutil_camera_direction
#define camera_right      GLutil_camera_right
#define camera_up         GLutil_camera_up
#define camera_basis      GLutil_camera_basis

#define camera_update      GLutil_camera_update
#define camera_update_step GLutil_camera_update_step

#define camera_orbiter        GLutil_camera_orbiter
#define orbiter_camera_update GLutil_orbiter_camera_update
#define camera_orbit          GLutil_camera_orbit

#define camera_first_person        GLutil_camera_first_person
#define first_person_camera_update GLutil_first_person_camera_update


#define add_light                   GLutil_add_light
#define get_light                   GLutil_get_light
#define generate_shadow_map_texture GLutil_generate_shadow_map_texture
#define activate_shadow_buffer      GLutil_activate_shadow_buffer
#define deactivate_shadow_buffer    GLutil_deactivate_shadow_buffer
#define light_space_matrix          GLutil_light_space_matrix
#define compute_shadow_maps         GLutil_compute_shadow_maps

#define print_framebuffer_status    GLutil_print_framebuffer_status

#endif // GLUTIL_INCLUDE_PREFIX




#endif // GLUTIL_H_
/**************************** FIM DO HEADER ***********************************/






#ifdef GLUTIL_IMPLEMENTATION


#define GLUTIL_DIV_EPS 0.000001

static const char* GLUTIL_RENDER_VERT_SHADER_PATH        = "./shaders/render_vert.glsl";
static const char* GLUTIL_RENDER_FRAG_SHADER_PATH        = "./shaders/render_frag.glsl";

static const char* GLUTIL_SHADOW_VERT_SHADER_PATH        = "./shaders/shadow_vert.glsl";
static const char* GLUTIL_SHADOW_FRAG_SHADER_PATH        = "./shaders/shadow_frag.glsl";

static const char* GLUTIL_COMPATIBILITY_VERT_SHADER_PATH = "./shaders/compatibility_vert.glsl";
static const char* GLUTIL_COMPATIBILITY_FRAG_SHADER_PATH = "./shaders/compatibility_frag.glsl";

static const char* GLUTIL_TEXTURE_VERT_SHADER_PATH = "./shaders/texture_vert.glsl";
static const char* GLUTIL_TEXTURE_FRAG_SHADER_PATH = "./shaders/texture_frag.glsl";


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
*                                                                              *
*      TABELA DE XMACRO PARA FUNÇÕES CARREGADAS DURANTE O RUNTIME              *
*                                                                              *
******************************************************************************/ 


//   function_name                return_type   ...params
#define GLUTIL_GL_FUNCTIONS_TO_LOAD                                                                                                                    \
   X( glXCreateContextAttribsARB, GLXContext,   Display*, GLXFBConfig, GLXContext, Bool, const int*                                                  ) \
   X( glGenBuffers,               void,         GLsizei n, GLuint *buffers                                                                           ) \
   X( glBindBuffer,               void,         GLenum target, GLuint  buffer                                                                        ) \
   X( glBufferData,               void,         GLenum target, GLsizeiptr size, const void *data, GLenum usage                                       ) \
   X( glGenFramebuffers,          void,         GLsizei n, GLuint* ids                                                                               ) \
   X( glBindFramebuffer,          void,         GLenum target, GLuint framebuffer                                                                    ) \
   X( glFramebufferTexture,       void, 	    GLenum target, GLenum attachment, GLuint texture, GLint level                                        ) \
   X( glFramebufferTexture1D,     void, 	    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level                      ) \
   X( glFramebufferTexture2D,     void, 	    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level                      ) \
   X( glFramebufferTexture3D,     void, 	    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer         ) \
   X( glCheckFramebufferStatus,   GLenum,       GLenum target                                                                                        ) \
   X( glGenRenderbuffers,         void,         GLsizei n, GLuint* renderbuffers                                                                     ) \
   X( glBindRenderbuffer,         void,         GLenum target, GLuint renderbuffer                                                                   ) \
   X( glRenderbufferStorage,      void,         GLenum target, GLenum internalformat, GLsizei width, GLsizei height                                  ) \
   X( glFramebufferRenderbuffer,  void,         GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer                     ) \
   X( glCreateShader,             GLuint,       GLenum type                                                                                          ) \
   X( glShaderSource,             void,         GLuint shader,  GLsizei count, const GLchar *const*string, const GLint *length                       ) \
   X( glCompileShader,            void,         GLuint shader                                                                                        ) \
   X( glAttachShader,             void,         GLuint program, GLuint shader                                                                        ) \
   X( glDeleteShader,             void,         GLuint shader                                                                                        ) \
   X( glCreateProgram,            GLuint,       void                                                                                                 ) \
   X( glGetShaderiv,              void,         GLuint shader, GLenum pname, GLint *params                                                           ) \
   X( glGetShaderInfoLog,         void,         GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog                                   ) \
   X( glGetProgramInfoLog,        void,         GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog                                  ) \
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
   X( glUniform2fv,               void,         GLint location, GLsizei count, const GLfloat* value                                                  ) \
   X( glUniform3fv,               void,         GLint location, GLsizei count, const GLfloat* value                                                  ) \
   X( glUniform4fv,               void,         GLint location, GLsizei count, const GLfloat* value                                                  ) \
   X( glUniformMatrix3fv,         void,         GLint location, GLsizei count, GLboolean transpose, const GLfloat* value                             ) \
   X( glUniformMatrix4fv,         void,         GLint location, GLsizei count, GLboolean transpose, const GLfloat* value                             ) \
   X( glUniformMatrix2x3fv,       void,         GLint location, GLsizei count, GLboolean transpose, const GLfloat* value                             )
    
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
*  (@FROM: https://github.com/ColleagueRiley/OpenGL-Context-Creation)          *
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
    GLutil_hide_cursor(glutil);


#ifndef GLUTIL_NO_GL_LEGACY // @NOTE: o debugger renderdoc não suporta essas funções legadas, por isso esse macro
    // permite usar a api legado do OpenGL, mas não afeta o uso de shaders
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    GLutil_ortho2D(0, w, 0, h); // coloca origem no topo esquerda
#endif

    glEnable(GL_DEPTH_TEST); // z-buffer
    
    // fica um pouco mais ~smooth~, menos pixelado
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLutil_camera_orbiter(glutil);
    glutil->input  = (GLutil_Input){0};

    glutil->lights = (GLutil_PointLights){};
    carr_vec_init(&glutil->lights);

    glutil->cur_t_nanos  = 0;
    glutil->prev_t_nanos = 0;
    glutil->dt_nanos     = 0;
    glutil->target_sleep_us = 16000; // 16 ms => 60 FPS


    GLutil_setup_fullscreen_buffer(glutil);
    glutil->render_shader        = GLutil_render_shaders();
    glutil->shadow_shader        = GLutil_shadow_shaders();
    glutil->compatibility_shader = GLutil_compatibility_shaders();
    glutil->texture_shader       = GLutil_texture_shaders();
    GLutil_shader_use(glutil, render_shaders());

}

void GLutil_destroy(GLutil* glutil)
{
    glXDestroyContext(glutil->display, glutil->ctx);
    XCloseDisplay(glutil->display);
}


void GLutil_hide_cursor(GLutil* glutil) {
    Pixmap blank;
    XColor dummy;
    char data[1] = {0};

    blank = XCreateBitmapFromData(glutil->display, glutil->window, data, 1, 1);
    Cursor cursor = XCreatePixmapCursor(glutil->display, blank, blank, &dummy, &dummy, 0, 0);
    XDefineCursor(glutil->display, glutil->window, cursor);
    int status = XGrabPointer(
        glutil->display,
        glutil->window,
        False,
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        glutil->window, // confine_to: keep pointer inside window
        cursor,
        CurrentTime
    );
    XFreePixmap(glutil->display, blank);
    XFreeCursor(glutil->display, cursor);
    XFlush(glutil->display);
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

float GLutil_get_time_secs(GLutil* glutil)
{
    return ( (float)glutil->cur_t_nanos / 1000.0f / 1000.0f / 1000.0f );
}

uint64_t GLutil_frame_delta_nanos(GLutil *glutil)
{
    return glutil->dt_nanos;
}

uint64_t GLutil_begin_frame(GLutil *glutil)
{
    glutil->prev_t_nanos = glutil->cur_t_nanos;
    glutil->cur_t_nanos = GLutil_get_time_nanos();
    GLutil_update_input(glutil);
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

void GLutil_unlimited_fps(GLutil* glutil)
{
    glutil->target_sleep_us = (uint64_t)0;
}


bool GLutil_next_event(GLutil* glutil, XEvent* event)
{
    return XCheckMaskEvent( glutil->display, ALL_INPUT_MASKS , event );
}



/*******************************************************************************
 *                     VETORES E ALGEBRA LINEAR                                *
 *******************************************************************************/

// vetores

GLutil_Vec2i GLutil_Vec2i_add(GLutil_Vec2i a, GLutil_Vec2i b) 
{
    return (GLutil_Vec2i) {
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
}

GLutil_Vec2i GLutil_Vec2i_sub(GLutil_Vec2i a, GLutil_Vec2i b)
{
    return (GLutil_Vec2i) {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
}

int GLutil_Vec2i_sqlength(GLutil_Vec2i v)
{
    // c² = a² + b²
    return (v.x*v.x + v.y*v.y);
}

float GLutil_Vec2i_length(GLutil_Vec2i v)
{
    // c = sqrtf(a² + b²)
    return sqrtf(GLutil_Vec2i_sqlength(v));
}

GLutil_Vec2i GLutil_Vec2i_rotccw(GLutil_Vec2i v, float theta)
{
        float sine   = sinf(theta);
        float cosine = cosf(theta);
        
        GLutil_Vec2i v_rot = (GLutil_Vec2i) {
            .x = + v.x * cosine + v.y * sine,
            .y = + v.y * cosine - v.x * sine,
        };

        return v_rot;
}

GLutil_Vec2i GLutil_Vec2i_rotcw(GLutil_Vec2i v, float theta)
{
        float sine   = sinf(theta);
        float cosine = cosf(theta);
        
        GLutil_Vec2i v_rot = (GLutil_Vec2i) {
            .x = + v.x * cosine - v.y * sine,
            .y = + v.y * cosine + v.x * sine,
        };

        return v_rot;
}

int GLutil_Vec2i_dot(GLutil_Vec2i a, GLutil_Vec2i b)
{
    return  a.x * b.x + a.y * b.y;
}


GLutil_Vec2i GLutil_Vec2i_scale(GLutil_Vec2i v, float sx, float sy)
{
    return (GLutil_Vec2i) {
        .x = v.x * sx,
        .y = v.y * sy,
    };
}


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

GLutil_Vec3f GLutil_Vec3f_div(GLutil_Vec3f v, float d)
{
    // @NOTE: não tenho certeza sobre qual valor utilizar como retorno
    //        quando d = 0
    //        opção1: Vec3f { 0, 0, 0 } // vetor nulo
    //        opção2: Vec3f { MAX_VAL, MAX_VAL, MAX_VAL } // maior vetor possível
    //        acho a opção 2 faz mais sentido, dado que o lim (1/x) = +inf
    //                                                    x->0

    const float MAX_VAL = 999999999999.9;
    if (d < GLUTIL_DIV_EPS) {
        return (GLutil_Vec3f) { MAX_VAL, MAX_VAL, MAX_VAL };
        // return (GLutil_Vec3f){0 , 0, 0 };
    }
    return (GLutil_Vec3f) {
        .x = v.x / d,
        .y = v.y / d,
        .z = v.z / d,
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

float GLutil_Vec3f_angle_between(GLutil_Vec3f a, GLutil_Vec3f b)
{
    // AºB = |A| * |B| * cos(theta)
    // cos(theta) = AºB / |A|*|B|
    // theta = arccos(AºB / |A|*|B|)
    float la = GLutil_Vec3f_length(a); // |A|
    float lb = GLutil_Vec3f_length(b); // |B|
    float d =  GLutil_Vec3f_dot(a, b);  // AºB
    float cosine = d / (la*lb); // AºB / |A|*|B|
    return acosf(cosine);       // arccos(AºB / |A|*|B|)
}

GLutil_Vec3f GLutil_Vec3f_apply_mat3x3(GLutil_Vec3f v, Mat3x3f A)
{
    //  | A00  A01 | * | vx | =>  | dot( {A00, A01}, v ) |
    //  | A10  A11 |   | vy |     | dot( {A10, A11}, v ) |
    return (GLutil_Vec3f) {
        .x = GLutil_Vec3f_dot(A.rows[0], v),
        .y = GLutil_Vec3f_dot(A.rows[1], v),
        .z = GLutil_Vec3f_dot(A.rows[2], v),
    };
}

GLutil_Vec3f GLutil_Vec3f_rotate_around_axis(GLutil_Vec3f v, GLutil_Vec3f R, float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);

    // @FROM: https://learnopengl.com/Getting-started/Transformations
    Mat4x4f m;
    m._11 = R.x*R.x*(1-c) + c;
    m._12 = R.x*R.y*(1-c) - R.z*s;
    m._13 = R.x*R.z*(1-c) + R.y*s;
    m._14 = 0.0f;
    m._21 = R.y*R.x*(1-c) + R.z*s;
    m._22 = R.y*R.y*(1-c) + c;
    m._23 = R.y*R.z*(1-c) - R.x*s;
    m._24 = 0.0f;
    m._31 = R.z*R.x*(1-c) - R.y*s;
    m._32 = R.z*R.y*(1-c) + R.x*s;
    m._33 = R.z*R.z*(1-c) + c;
    m._44 = 0.0f;
    m._41 = 0.0f;
    m._42 = 0.0f;
    m._43 = 0.0f;
    m._44 = 1.0f;

    GLutil_Vec4f v4 = { .x = v.x, .y = v.y, .z = v.z, .w = 1.0f };
    v4 = GLutil_Vec4f_apply_mat4x4(v4, m);
    return (Vec3f) { .x = v4.x, .y = v4.y, .z = v4.z };
}


GLutil_Vec3f GLutil_Vec3f_rotate_x(GLutil_Vec3f v, float angle)
{
    float cosine = cosf(angle);
    float sine = sinf(angle);
    GLutil_Mat3x3f rot_x = {
            1.0f, 0.0,     0.0,
            0.0f, cosine, -sine,
            0.0f, sine,    cosine,
    };
    return GLutil_Vec3f_apply_mat3x3(v, rot_x);
}

GLutil_Vec3f GLutil_Vec3f_rotate_y(GLutil_Vec3f v, float angle)
{
    float cosine = cosf(angle);
    float sine = sinf(angle);
    GLutil_Mat3x3f rot_y = {
       cosine, 0.0f,  sine,
       0.0,    1.0f,  0.0,
      -sine,   0.0f,  cosine, 
    };
    return GLutil_Vec3f_apply_mat3x3(v, rot_y);
}

GLutil_Vec3f GLutil_Vec3f_rotate_z(GLutil_Vec3f v, float angle)
{
    float cosine = cosf(angle);
    float sine = sinf(angle);
    GLutil_Mat3x3f rot_z = {
        cosine, -sine,   0.0f, 
        sine,    cosine, 0.0f, 
        0.0,     0.0,    1.0f, 
    };
    return GLutil_Vec3f_apply_mat3x3(v, rot_z);
}



float GLutil_Vec4f_dot(GLutil_Vec4f a, GLutil_Vec4f b)
{
    return a.x * b.x  + a.y * b.y + a.z * b.z  + a.w * b.w;
}

GLutil_Vec4f GLutil_Vec4f_apply_mat4x4(GLutil_Vec4f v, Mat4x4f A)
{
    return (GLutil_Vec4f) {
        .x = GLutil_Vec4f_dot(A.rows[0], v),
        .y = GLutil_Vec4f_dot(A.rows[1], v),
        .z = GLutil_Vec4f_dot(A.rows[2], v),
        .w = GLutil_Vec4f_dot(A.rows[3], v),
    };
}


// matrizes
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

GLutil_Mat4x4f GLutil_Mat4x4f_transpose(GLutil_Mat4x4f A)
{
    GLutil_Mat4x4f m;
    m._11 = A.rows[0].x;
    m._21 = A.rows[0].y;
    m._31 = A.rows[0].z;
    m._41 = A.rows[0].w;

    m._12 = A.rows[1].x;
    m._22 = A.rows[1].y;
    m._32 = A.rows[1].z;
    m._42 = A.rows[1].w;

    m._13 = A.rows[2].x;
    m._23 = A.rows[2].y;
    m._33 = A.rows[2].z;
    m._43 = A.rows[2].w;

    m._14 = A.rows[3].x;
    m._24 = A.rows[3].y;
    m._34 = A.rows[3].z;
    m._44 = A.rows[3].w;
    
    return m;
}


GLutil_Mat4x4f GLutil_Mat4x4f_mul(GLutil_Mat4x4f A, GLutil_Mat4x4f B)
{
    // @FROM: https://github.com/tsoding/la/blob/master/la.h
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


// quaternions
GLutil_Quaternion GLutil_unit_quaternion() // no rotation
{
    return (GLutil_Quaternion) {
        // vector part == 0.0f
        .x = 0.0f,
        .y = 0.0f, 
        .z = 0.0f,
        // real part == 1.0f
        .w = 1.0f,
    };
}

GLutil_Quaternion GLutil_quat_from_angle_axis(float angle, GLutil_Vec3f axis)
{
    GLutil_Quaternion quat;
    float s = sinf(angle / 2.0f);
    float c = cosf(angle / 2.0f);

    quat.w = c;
    quat.x = s * axis.x;
    quat.y = s * axis.y;
    quat.z = s * axis.z;
    return quat;
}

GLutil_Quaternion GLutil_quat_from_vec3f(GLutil_Vec3f v)
{
    return (GLutil_Quaternion) { .w = 0.0f, .x = v.x, .y = v.y, .z = v.z };
}

int GLutil_quat_sqlength(GLutil_Quaternion q)
{
    return (q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

GLutil_Quaternion GLutil_quat_conjugate(GLutil_Quaternion q)
{
    return (GLutil_Quaternion) { .x = -q.x, .y = -q.y, .z = -q.z, .w = q.w };
}

GLutil_Quaternion GLutil_quat_scale(GLutil_Quaternion q, float s)
{
    return (GLutil_Quaternion) {
        .w = q.w * s,
        .x = q.x * s,
        .y = q.y * s, 
        .z = q.z * s,
    };
}

GLutil_Quaternion GLutil_quat_div_scalar(GLutil_Quaternion v, float d)
{
    if ( d == 0.0f ) {
        printf("[WARNING] quat_div_scalar: Attempt to divide by 0. returning original quaternion without changes.\n");
        return v;
    }
    float s = 1.0f / d;
    return GLutil_quat_scale(v, s);
}

GLutil_Quaternion GLutil_quat_mul(GLutil_Quaternion a_, GLutil_Quaternion b_)
{
// @FROM: https://en.wikipedia.org/wiki/GLutil_Quaternions_and_spatial_rotation#GLutil_Quaternions
    float a = a_.w;
    float b = a_.x;
    float c = a_.y;
    float d = a_.z;
    float e = b_.w;
    float f = b_.x;
    float g = b_.y;
    float h = b_.z;

    GLutil_Quaternion q = {0};

    q.w = (a*e - b*f - c*g - d*h);
    q.x = (a*f + b*e + c*h - d*g);
    q.y = (a*g - b*h + c*e + d*f);
    q.z = (a*h + b*g - c*f + d*e);
    return q;
}


GLutil_Quaternion GLutil_quat_inverse(GLutil_Quaternion q)
{
    return GLutil_quat_div_scalar(GLutil_quat_conjugate(q), GLutil_quat_sqlength(q));
}

GLutil_Vec3f GLutil_Vec3f_rotate(GLutil_Vec3f v, float angle, GLutil_Vec3f axis)
{
    GLutil_Quaternion v_quat   = GLutil_quat_from_vec3f(v);
    GLutil_Quaternion q        = GLutil_quat_from_angle_axis(angle/2.0f, axis);
    GLutil_Quaternion qinv     = GLutil_quat_from_angle_axis(-angle/2.0f, axis);
    GLutil_Quaternion left_mul = GLutil_quat_mul(q, v_quat);
    GLutil_Quaternion result   = GLutil_quat_mul(left_mul, qinv);
    return (GLutil_Vec3f){ 
        .x = result.x,
        .y = result.y,
        .z = result.z 
    };
}


/*******************************************************************************
 *                     DESENHAR FORMAS GEOMÉTRICAS (OpenGL 1.1)                *
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
*                             SHADERS                                          *
********************************************************************************/

// O ShaderProgram padrão usado pra rasterizar a cena final que aparece no monitor
GLutil_ShaderProgram* GLutil_render_shaders(void)
{
    static GLutil_ShaderProgram render_shader = { .id = -1 };
    if ( render_shader.id == -1 ) {
        render_shader = GLutil_shader_new(GLUTIL_RENDER_VERT_SHADER_PATH, GLUTIL_RENDER_FRAG_SHADER_PATH);
        printf("Generated Render Shader with id = %u\n", render_shader.id);
    } 
    return &render_shader;
}

// ShaderProgram utilizado pra renderizar a cena em uma textura feita para servir como shadow_map
GLutil_ShaderProgram* GLutil_shadow_shaders(void)
{
    static GLutil_ShaderProgram shadow_shader = { .id = -1 };
    if ( shadow_shader.id == -1 ) {
        shadow_shader = GLutil_shader_new(GLUTIL_SHADOW_VERT_SHADER_PATH, GLUTIL_SHADOW_FRAG_SHADER_PATH);
        printf("Generated Render Shader with id = %u\n", shadow_shader.id);
    }
    return &shadow_shader;
}


// ShaderProgram pra rasterizar a cena a partir das funções legadas de modo imediato do OpenGL 2.1
GLutil_ShaderProgram*  GLutil_compatibility_shaders() 
{
    static GLutil_ShaderProgram compat_shader = { .id = 0 };
    if ( compat_shader.id == 0 ) {
        compat_shader = GLutil_shader_new(GLUTIL_COMPATIBILITY_VERT_SHADER_PATH, GLUTIL_COMPATIBILITY_FRAG_SHADER_PATH);
        printf("Generated Compatibility Shader with id = %u\n", compat_shader.id);
    }
    return &compat_shader;
}

GLutil_ShaderProgram* GLutil_texture_shaders()
{
    static GLutil_ShaderProgram texture_shader = { .id = 0 };
    if ( texture_shader.id == 0 ) {
        texture_shader = GLutil_shader_new(GLUTIL_TEXTURE_VERT_SHADER_PATH, GLUTIL_TEXTURE_FRAG_SHADER_PATH );
        printf("Generated Texture Shader with id = %u\n", texture_shader.id);
    }
    return &texture_shader;
}


GLutil_ShaderProgram GLutil_shader_new(const char* vert_shader, const char* frag_shader)
{
    GLutil_ShaderProgram shader = {};

    shader.vert_shader_filename = vert_shader; // salvar o nome pra bom error reporting
    GLuint vs_id = GLutil_shader_load_vert(vert_shader);
        
    shader.frag_shader_filename = frag_shader; // salvar o nome pra bom error reporting
    GLuint fs_id = GLutil_shader_load_frag(frag_shader);

    shader.id = glCreateProgram();
    glAttachShader(shader.id, vs_id); GLUTIL_CHECK_GL_ERROR();
    glAttachShader(shader.id, fs_id); GLUTIL_CHECK_GL_ERROR();
    glLinkProgram(shader.id); GLUTIL_CHECK_GL_ERROR();
    GLutil_check_shader_compilation_error(shader.id, "<anonymous_shader_program>", GLUTIL_PROGRAM_SHADER );
    glDeleteShader(vs_id); GLUTIL_CHECK_GL_ERROR();
    glDeleteShader(fs_id); GLUTIL_CHECK_GL_ERROR();
    return shader;
}

void GLutil_shader_reload(GLutil_ShaderProgram* shader)
{
    if ( shader->id > 0 ) {
        glDeleteProgram( shader->id );
    }
    *shader = GLutil_shader_new(shader->vert_shader_filename, shader->frag_shader_filename);
}

GLuint GLutil_shader_load_vert(const char* name)
{
    return GLutil_shader_load(name, GLUTIL_VERT_SHADER);
}

GLuint GLutil_shader_load_frag(const char* name)
{
    return GLutil_shader_load(name, GLUTIL_FRAG_SHADER);
}

GLuint GLutil_shader_load(const char* name, GLutil_ShaderType type)
{
    printf("Compiling Shader '%s'\n", name);
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
    check_shader_compilation_error(shader_id, name, type);
    printf("       ╰─ OK.\n");
    return shader_id;
}


void GLutil_check_shader_compilation_error_(GLuint shader_id, const char* shader_name, GLutil_ShaderType type, int line, const char* file, const char* func)
{

    // @FROM: adaptado de https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h 
    int success;
    char info_log[1024];
    if (type != GLUTIL_PROGRAM_SHADER) {
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader_id, 1024, NULL, info_log);
            fprintf(stderr, "%d:%s:%s [ERROR] Compilation of Shader '%s' failed: %s\n", line, file, func, shader_name, info_log);
            exit(6);
        }
    }
    else {
        glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_id, 1024, NULL, info_log);
            fprintf(stderr, "%d:%s:%s [ERROR] Linking of Shader '%s' failed: %s\n", line, file, func, shader_name, info_log);
            exit(7);
        }
    }
}



void GLutil_shader_use(GLutil* glutil, GLutil_ShaderProgram* shader)
{
    glutil->active_shader = shader;
    glUseProgram(shader->id);
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_int(GLutil_ShaderProgram shader, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(shader.id, name), value);
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_float(GLutil_ShaderProgram shader, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(shader.id, name), value);
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_vec2f(GLutil_ShaderProgram shader, const char* name, GLutil_Vec2f value)
{
    glUniform2fv(glGetUniformLocation(shader.id, name), 1, (float*)&value);
    GLUTIL_CHECK_GL_ERROR();
}


void GLutil_shader_add_vec3f(GLutil_ShaderProgram shader, const char* name, GLutil_Vec3f value)
{
    glUniform3fv(glGetUniformLocation(shader.id, name), 1, (float*)&value);
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_glcolor(GLutil_ShaderProgram shader, const char* name, GLutil_GLColor value)
{
    glUniform3fv(glGetUniformLocation(shader.id, name), 1, (float*)&value);
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_vec4f(GLutil_ShaderProgram shader, const char* name, GLutil_Vec4f value)
{
    glUniform4fv(glGetUniformLocation(shader.id, name), 1, (float*)&value);
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_quaternion(GLutil_ShaderProgram shader, const char* name, GLutil_Quaternion value)
{
    glUniform4fv(glGetUniformLocation(shader.id, name), 1, (float*)&value);
    GLUTIL_CHECK_GL_ERROR();
}


void GLutil_shader_add_mat3f(GLutil_ShaderProgram shader, const char* name, GLutil_Mat3x3f value)
{
    glUniformMatrix3fv(
        glGetUniformLocation(shader.id, name),
        1, GL_FALSE, value.data
    );
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_mat4f(GLutil_ShaderProgram shader, const char* name, GLutil_Mat4x4f value)
{
    glUniformMatrix4fv(
        glGetUniformLocation(shader.id, name),
        1, GL_FALSE, value.data
    );
    GLUTIL_CHECK_GL_ERROR();
}

void GLutil_shader_add_mat4f_transposed(GLutil_ShaderProgram shader, const char* name, GLutil_Mat4x4f value)
{
    glUniformMatrix4fv(
        glGetUniformLocation(shader.id, name),
        1, GL_TRUE, value.data
    );
    GLUTIL_CHECK_GL_ERROR();
}


// Funçõezinhas de compatibilidade com modo imediato
static GLutil_ShaderProgram* __shader_backup;
void GLutil_immediate_mode_begin(GLutil* glutil)
{
    __shader_backup = glutil->active_shader;
    GLutil_shader_use(glutil, GLutil_compatibility_shaders());
}

void GLutil_immediate_mode_end(GLutil* glutil)
{
    GLutil_shader_use(glutil, __shader_backup);
}

/*******************************************************************************
 *                               CAMERA                                        *
 ******************************************************************************/

void GLutil_orbiter_camera_update(GLutil* glutil)
{
// @TODO: deveriam ser parâmetros da câmera
    const float mouse_sensiv = 0.03f;
    const float step_size    = 0.5f;


    if ( glutil->input.mouse_button_right.pressed && glutil->input.mouse_movement.moved ) { 
        float cam_dist = Vec3f_length(Vec3f_sub(glutil->camera.pos, glutil->camera.target));
        Vec2i delta = Vec2i_sub(glutil->input.mouse_movement.pos, glutil->input.mouse_movement.last_pos);
        float theta = -mouse_sensiv * atan2f( delta.y, cam_dist );
        float phi =   -mouse_sensiv * atan2f( delta.x, cam_dist );
        glutil->camera = camera_orbit(glutil->camera, theta, phi);
    }

    GLutil_camera_update_step(glutil, step_size);
}

void GLutil_first_person_camera_update(GLutil* glutil)
{
    const float mouse_sensiv = 0.08f;
    const float step_size    = 0.5f;

    if ( glutil->input.mouse_movement.moved ) { 
        float cam_dist = Vec3f_length(Vec3f_sub(glutil->camera.pos, glutil->camera.target));
        Vec2i delta = Vec2i_sub(glutil->input.mouse_movement.pos, glutil->input.mouse_movement.last_pos);
        float theta = -mouse_sensiv * atan2f( delta.y, cam_dist );
        float phi =   -mouse_sensiv * atan2f( delta.x, cam_dist );


        GLutil_Camera cam = glutil->camera;
        GLutil_3DBasis cam_basis = GLutil_camera_basis(&cam);

        Vec3f rotated_x = GLutil_Vec3f_sub(cam.target, cam.pos);
        Vec3f rotated_y = rotated_x;

        float zenith = fabs(
            GLUTIL_RAD2DEG(
                GLutil_Vec3f_angle_between(
                    GLutil_Vec3f_add(cam.pos, cam_basis.front),
                    cam.up
                )
            )
        );
        if ( 1.0f < zenith && zenith < 180.0f ) {
            rotated_x = GLutil_Vec3f_rotate_around_axis(rotated_x, cam_basis.right, theta);
            rotated_y = rotated_x;
        }
        rotated_y = GLutil_Vec3f_rotate_around_axis(rotated_y, cam.up , phi);

        glutil->camera.target = GLutil_Vec3f_add(cam.pos, rotated_y);
    }

    GLutil_camera_update_step(glutil, step_size);
}



#define GLUTIL_ORBITER_CAM_FOV  90.0f
#define GLUTIL_ORBITER_CAM_NEAR 0.1f
#define GLUTIL_ORBITER_CAM_FAR  1000.0f
void GLutil_camera_orbiter(GLutil* glutil)
{
    GLutil_Camera orbiter = {
        .pos =    { .x = 0.0f, .y = 0.0f, .z = 30.0f },
        .target = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        .up =     { .x = 0.0f, .y = 1.0f, .z = 0.0f },

        .fovX = GLUTIL_ORBITER_CAM_FOV,
        .near = GLUTIL_ORBITER_CAM_NEAR,
        .far  = GLUTIL_ORBITER_CAM_FAR,

        .update = GLutil_orbiter_camera_update,
    };
    glutil->camera = orbiter;
}

#define GLUTIL_FIRST_PERSON_CAM_FOV  90.0f
#define GLUTIL_FIRST_PERSON_CAM_NEAR 0.1f
#define GLUTIL_FIRST_PERSON_CAM_FAR  1000.0f
void GLutil_camera_first_person(GLutil* glutil)
{
    GLutil_Camera fps = {
        .pos =    { .x = 0.0f, .y = 0.0f, .z = 30.0f },
        .target = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
        .up =     { .x = 0.0f, .y = 1.0f, .z = 0.0f },

        .fovX = GLUTIL_FIRST_PERSON_CAM_FOV,
        .near = GLUTIL_FIRST_PERSON_CAM_NEAR,
        .far  = GLUTIL_FIRST_PERSON_CAM_FAR,

        .update = GLutil_first_person_camera_update,
    };
    glutil->camera = fps; 
}

void GLutil_camera_update_step(GLutil* g, float step_size)
{
// @TODO: abstrair quais teclas correspondem a quais ações.

    if ( g->input.arrow_up.pressed || g->input.w.pressed ) {
        Vec3f dir = GLutil_camera_direction(&g->camera);
        Vec3f step = Vec3f_scale(dir, step_size);
        g->camera.pos = Vec3f_add(g->camera.pos, step);
    }

    if ( g->input.arrow_down.pressed || g->input.s.pressed ) {
        Vec3f dir = GLutil_camera_direction(&g->camera);
        Vec3f step = Vec3f_scale(dir, step_size);
        g->camera.pos = Vec3f_sub(g->camera.pos, step);
    };

    if ( g->input.arrow_right.pressed || g->input.d.pressed ) {
        Vec3f right = GLutil_camera_right(&g->camera);
        Vec3f step = Vec3f_scale(right, step_size);
        g->camera.pos = Vec3f_add(g->camera.pos, step);
    }

    if ( g->input.arrow_left.pressed || g->input.a.pressed ) {
        Vec3f right = GLutil_camera_right(&g->camera);
        Vec3f step = Vec3f_scale(right, step_size);
        g->camera.pos = Vec3f_sub(g->camera.pos, step);
    }
}



void GLutil_camera_update(GLutil* glutil)
{
    if ( glutil->camera.update ) glutil->camera.update(glutil);
}


GLutil_Mat3x3f GLutil_model_matrix(GLutil_Camera cam)
{
    return (GLutil_Mat3x3f) {
        .data = {                                    
            1, 0, 0, 
            0, 1, 0, 
            0, 0, 1    
        }
    };
}

GLutil_Mat4x4f GLutil_view_matrix(GLutil_Camera cam)
{

    GLutil_Vec3f dir = GLutil_Vec3f_sub(cam.pos, cam.target);
    dir = GLutil_Vec3f_normalize(dir);

    GLutil_Vec3f right = GLutil_Vec3f_cross(cam.up, dir);
    right = GLutil_Vec3f_normalize(right);

    GLutil_Vec3f up = GLutil_Vec3f_cross(dir, right);
    up = GLutil_Vec3f_normalize(up);

    // @FROM: https://github.com/lukeflima/Segundo-Trabalho/blob/master/Segundo-Trabalho/mygl.h 
    GLutil_Mat4x4f lukeat = (GLutil_Mat4x4f) {
        .data = {
            right.x, right.y, right.z, 0,
            up.x,    up.y,    up.z,    0,
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


GLutil_Vec3f GLutil_camera_direction(GLutil_Camera* camera)
{
    GLutil_Vec3f dir = GLutil_Vec3f_sub(camera->target, camera->pos);
    return GLutil_Vec3f_normalize(dir);
}

GLutil_Vec3f GLutil_camera_right(GLutil_Camera* camera)
{
    GLutil_Vec3f dir = GLutil_camera_direction(camera);
    GLutil_Vec3f right = GLutil_Vec3f_cross(dir, camera->up);
    return GLutil_Vec3f_normalize(right);
}

GLutil_Vec3f GLutil_camera_up(GLutil_Camera* camera)
{
    GLutil_Vec3f dir = GLutil_camera_direction(camera);
    GLutil_Vec3f right = GLutil_camera_right(camera);
    GLutil_Vec3f up = Vec3f_normalize(Vec3f_cross(right, dir));
    return up;
}


GLutil_3DBasis GLutil_camera_basis(GLutil_Camera* camera)
{
    GLutil_Vec3f dir = GLutil_camera_direction(camera);                // aponta da camera pra alvo, atua como eixo z local 
    GLutil_Vec3f right = GLutil_camera_right(camera);                  // perpendicular a dir e ao 'up' global, atua como x local
    GLutil_Vec3f up = Vec3f_normalize(Vec3f_cross(right, dir)); // perpendicular ao dois primeiros, atua como y local
    return (GLutil_3DBasis) {
        .front = dir, .right = right, .up = up,
    };
}


GLutil_Camera GLutil_camera_orbit(GLutil_Camera camera, float theta, float phi)
{
    GLutil_Camera out = camera;
    GLutil_3DBasis cam_basis = GLutil_camera_basis(&out);

    GLutil_Vec3f rotated_x = out.pos;
    GLutil_Vec3f rotated_y = out.pos;

    // @TODO: aparentemente quando a câmera fica alinhada com o eixo vertical
    //        isso causa um "lookAt flip". pesquisar de quais outras formas
    //        costumam contornar essa situação além de simplesmente ignorar
    //        a rotação caso a câmera esteja muito perto do eixo.
    float zenith = fabs(GLUTIL_RAD2DEG(GLutil_Vec3f_angle_between(out.pos, out.up)));
    if ( 1.0f < zenith && zenith < 180.0f ) {
        rotated_x = GLutil_Vec3f_rotate_around_axis(rotated_x, cam_basis.right, theta);
        rotated_y = rotated_x;
    }
    rotated_y = GLutil_Vec3f_rotate_around_axis(rotated_y, cam_basis.up , phi);
    out.pos = rotated_y;
    return out;
}

/*******************************************************************************
 *                            LIGHTING                                         *
*******************************************************************************/
int GLutil_add_light(GLutil* glutil, GLutil_PointLight light) 
{
    // Retorna o índice da instância de light no vec de lights
    carr_vec_append(&glutil->lights, light);
    int light_id = glutil->lights.len - 1;
    GLutil_generate_shadow_map_texture(get_light(glutil, light_id));
    return light_id;
}

GLutil_PointLight* GLutil_get_light(GLutil* glutil, int light_id) 
{
    // Retorna o pointer pra instância de light no vec de lights
    return &glutil->lights.items[light_id];
}


void GLutil_generate_shadow_map_texture(GLutil_PointLight* l)
{
#define SHADOW_WIDTH  1024
#define SHADOW_HEIGHT 1024
    printf("Generating Shadow Map Texture.\n");
    glGenTextures( 1, &l->shadow_map ); CHECK_GL_ERROR();
    glBindTexture( GL_TEXTURE_2D, l->shadow_map ); CHECK_GL_ERROR();
    glTexImage2D(
        GL_TEXTURE_2D, 0,               
        GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, 
        NULL 
    ); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER); CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); CHECK_GL_ERROR();
    float border_clr[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_clr); CHECK_GL_ERROR();

    printf("       ╰─ OK. ShadowMap TextureId = %d\n", l->shadow_map);
    printf("Generating Shadow Framebuffer\n");

    glGenFramebuffers(1, &l->shadow_buffer); CHECK_GL_ERROR();
    glBindFramebuffer( GL_FRAMEBUFFER, l->shadow_buffer ); CHECK_GL_ERROR();
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
        GL_TEXTURE_2D,  l->shadow_map, 0
    ); CHECK_GL_ERROR();
    glDrawBuffer( GL_NONE ); CHECK_GL_ERROR();
    glReadBuffer( GL_NONE ); CHECK_GL_ERROR();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 ); CHECK_GL_ERROR();
    printf("       ╰─ OK. Shadow FramebufferId = %d\n", l->shadow_buffer);
}

void GLutil_activate_shadow_buffer(GLutil* g, GLutil_PointLight* l)
{
    GLutil_Vec3f light_target = g->camera.target;
    GLutil_Vec3f forward = GLutil_Vec3f_normalize(GLutil_Vec3f_sub( light_target, l->pos )); // 1 unit long
    GLutil_Vec3f world_up = fabsf(forward.y) > 0.99 
        ? (GLutil_Vec3f){0.0f,0.0f,1.0f} : (GLutil_Vec3f){0.0f,1.0f,0.0f};

    GLutil_Vec3f right = GLutil_Vec3f_cross(forward, world_up);
    GLutil_Vec3f up    = GLutil_Vec3f_cross(right, forward);
    l->pov = (Camera) {
        .pos    = l->pos,
        .target = light_target,
        .up     = up,
        .fovX   = 60.0f,
        .near   = 1.0f,
        .far    = 200.0f
    };

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);      CHECK_GL_ERROR();
    glBindFramebuffer( GL_FRAMEBUFFER, l->shadow_buffer); CHECK_GL_ERROR();
    glClear( GL_DEPTH_BUFFER_BIT ); CHECK_GL_ERROR();
    glEnable( GL_DEPTH_TEST );
}

void GLutil_deactivate_shadow_buffer(GLutil* g, GLutil_PointLight* l, GLutil_Camera old_cam)
{
    g->camera = old_cam;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLUTIL_CHECK_GL_ERROR();
}


GLutil_Mat4x4f GLutil_light_space_matrix(GLutil_PointLight l)
{
     Mat4x4f light_mat = Mat4x4f_mul(
         Mat4x4f_transpose(view_matrix(l.pov)),
         Mat4x4f_transpose(projection_matrix(l.pov, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT))
     );
     return light_mat;
}


void GLutil_compute_shadow_maps(GLutil* g, int models_count, GLutil_Model** models)
{

    /**************  SHADOW MAP SETUP *********************************/
    GLutil_Camera cam = g->camera;
    int original_width = g->winwidth;
    int original_height = g->winheight;

    // necessário pra calcular o aspect_ratio que é passado pra projection_matrix, durante o render_model
    g->winwidth  = SHADOW_WIDTH;
    g->winheight = SHADOW_HEIGHT;


    GLutil_ShaderProgram* original_shader = g->active_shader;
    shader_use(g, GLutil_shadow_shaders());
    /**********************************************************************/


    for ( int j = 0; j < g->lights.len; ++j) {
        GLutil_PointLight* light = get_light(g, j);
        activate_shadow_buffer(g, light); // ativa o framebuffer da light e calcula o pov dela
        g->camera = light->pov;           // muda a camera global pra perspectiva da light

        /**************** RENDER EACH MODEL IN LIGHT'S POV ********************/
        for (  int i = 0; i < models_count; ++i ) {
            GLutil_Model* model = models[i];
            render_model(g, model);
        }
        /**********************************************************************/

        deactivate_shadow_buffer(g, light, cam);
    }

    /*************** RESTORE STATE TO BEFORE SHADOW MAP RENDERING *************/
    glViewport(0, 0, original_width, original_height);      
    CHECK_GL_ERROR();

    shader_use(g, original_shader);
    g->camera    = cam;
    g->winwidth  = original_width;
    g->winheight = original_height;
    /**************************************************************************/
}




/*******************************************************************************
 *                 INPUT (MOUSE, KEYBOARD, ETC)                                *
 ******************************************************************************/
void GLutil_update_input(GLutil* glutil)
{
#define X(_, field) \
    glutil->input.field.down = false;
X11_KEYBOARD_EVENTS
#undef X


    // MOUSE EVENTS. ainda não foram automatizados com XMACROS.
    glutil->input.mouse_button_left.down = false;
    glutil->input.mouse_button_right.down = false;

    glutil->input.mouse_movement.moved = false;
    glutil->input.mouse_movement.last_pos = glutil->input.mouse_movement.pos;


    while (XPending(glutil->display)) {
        XEvent event;
        XNextEvent(glutil->display, &event);

        if (event.type == KeyPress) {
#define X(x11_symbol, field) \
            if (event.xkey.keycode == x11_symbol ) { \
                glutil->input.field.down    = true;  \
                glutil->input.field.pressed = true;  \
            }
X11_KEYBOARD_EVENTS
#undef X

        }

        if (event.type == KeyRelease) {
#define X(x11_symbol, field) \
            if (event.xkey.keycode == x11_symbol ) { \
                glutil->input.field.down    = false;  \
                glutil->input.field.pressed = false;  \
            }
X11_KEYBOARD_EVENTS
#undef X
        }

        if (event.type == ButtonPress) {
            if (event.xbutton.button == X11_MOUSE_BUTTON_RIGHT) {
                glutil->input.mouse_button_right.down    = true;
                glutil->input.mouse_button_right.pressed = true;
            }
            if (event.xbutton.button == X11_MOUSE_BUTTON_LEFT) {
                glutil->input.mouse_button_left.down    = true;
                glutil->input.mouse_button_left.pressed = true;
            }
        }

        if (event.type == ButtonRelease) {
            if (event.xbutton.button == X11_MOUSE_BUTTON_RIGHT) {
                glutil->input.mouse_button_right.down    = false;
                glutil->input.mouse_button_right.pressed = false;
            }
            if (event.xbutton.button == X11_MOUSE_BUTTON_LEFT) {
                glutil->input.mouse_button_left.down    = false;
                glutil->input.mouse_button_left.pressed = false;
            }
        }

        if (event.type == MotionNotify) {
            glutil->input.mouse_movement.moved = true;
            glutil->input.mouse_movement.pos.x = event.xmotion.x;
            glutil->input.mouse_movement.pos.y = event.xmotion.y;
        }
    }
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

typedef struct {
    GLutil_Vec3f* items;
    size_t        len;
    size_t        cap;
} Normals;


ObjData GLutil_load_obj_file(const char* file_path)
{
    printf("Loading Obj Data from '%s'\n", file_path);

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

    Normals vns = {0};  // temp vec for normals
    carr_vec_init(&vns);

    Faces v_idxs = {0};   // temp vec for vertices indexes for faces
    carr_vec_init(&v_idxs);

    Faces vt_idxs = {0};  // temp vec for texture indixes for faces
    carr_vec_init(&vt_idxs);

    Faces vn_idxs = {0};  // temp vec for texture indixes for faces
    carr_vec_init(&vn_idxs);


    while (file.len > 0) {
        CarrStringView line = carr_sv_chop_line(&file);
        if (line.len == 0 || carr_sv_first(&line) == '#')
            continue;

        CarrStringView tag = carr_sv_chop_by_space(&line); 
        if ( carr_sv_is_equal(tag, OBJECT_TAG) || carr_sv_is_equal(tag, SMOOTH_TAG)) {
            printf("[WARNING] Vertex Data not supported: '%.*s'\n", (int)tag.len, tag.data);
        } else if ( carr_sv_is_equal(tag, NORMAL_TAG) ) {
            char linebuf[100] = {0};
            assert(line.len < 100);
            memcpy(linebuf, line.data, sizeof(char) * line.len);

            GLutil_Vec3f v;
            sscanf(linebuf, "%f %f %f", &v.x, &v.y, &v.z);
            carr_vec_append(&vns, v);

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
            
            // Cada string (%s) do scanf a seguir é composta por até 3 ints,
            // cada um representando um índice nos vetores de dados.
            // O primeiro representa o índice no vetor de vértices (x, y, z).
            // O segundo representa o índice no vetor de texturas  (u, v).
            // O terceito representa o índice no vetor de normais  (x, y,z).
            // O primeiro float está sempre presente, o segundo e o terceiro são opcionais.
            // Apenas índice de vértice:     %d
            // Índice de vértice e texture:  %d|%d
            // Índice de vértice e normal:   %d||%d // @TODO: lidar com esse caso corretamente
            // Todos os 3 índices:           %d|%d|%d  
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
                        carr_vec_append(&vn_idxs, idx_int - 1);
                    }
                }
            }

        } else {
            printf("[WARNING] Unknown Vertex Data Tag: '%.*s'\n", (int)tag.len, tag.data);
        }

    }


    if (vt_idxs.len != 0) assert(vt_idxs.len == v_idxs.len);
    if (vn_idxs.len != 0) assert(vn_idxs.len == v_idxs.len);

    for (int i = 0; i < v_idxs.len; ++i) {
        int vidx = v_idxs.items[i] * 3;
        

        float x = vs.items[vidx + 0];
        float y = vs.items[vidx + 1];
        float z = vs.items[vidx + 2];

        float u = 0.0f;
        float v = 0.0f;

        if (vt_idxs.len > 0) {
            int vtidx = vt_idxs.items[i] * 2;
            u = vts.items[vtidx + 0];
            v = vts.items[vtidx + 1];
        }

        GLutil_Vec3f vn;
        if (vn_idxs.len > 0) {
            int vn_idx = vn_idxs.items[i];
            vn = vns.items[vn_idx];
        }

        Vertex vertex = {
            .pos         = { .x = x,    .y = y,    .z = z,   },
            .color       = { .r = 1.0f, .g = 1.0f, .b = 1.0f },
            .tex_pos     = { .x = u,    .y = v               },
            .normal      = vn,
        };

        carr_vec_append(&verts, vertex);
        carr_vec_append(&faces, verts.len - 1);
    }

    carr_vec_free(&vs);
    carr_vec_free(&vts);
    carr_vec_free(&vns);
    carr_vec_free(&v_idxs);
    carr_vec_free(&vt_idxs);
    carr_vec_free(&vn_idxs);
    carr_sb_free(&sb);


    obj.vertices   = verts;
    obj.faces      = faces;

    printf("       ╰─ OK.\n");
    printf("Attaching Default Shaders to Obj Data.\n");
    GLutil_load_obj_to_GPU(&obj);
    
    return obj;
}

void GLutil_load_obj_to_GPU(GLutil_ObjData* obj)
{
    printf("Loading Obj Data to GPU.\n");
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
    GLuint attrb_loc = 0;

    // @TODO: o cálculo da quantidadde de elemenntos por atributo vai quebrar caso se altere o tipo do elemento individual.
    //        isto é, seria bom descobrir como fazer algo que não depende explicitamente do tipo 'float' ou qualquer outro tipo hardcoded.
    //        algo como:
    //        sizeof(( *obj->vertices.items ).pos) / sizeof( ( typeof( (*obj->vertices.items ).pos.__first_field__ ) ) )
    //        mas claro, algo como o campo __first_field__ não existe e não é assim que o macro typeof funciona (eu acho?).
    glVertexAttribPointer(attrb_loc, sizeof((*obj->vertices.items).pos)         / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(*obj->vertices.items), (void*)offsetof(GLutil_Vertex, pos));
    glEnableVertexAttribArray(attrb_loc++);

    glVertexAttribPointer(attrb_loc, sizeof((*obj->vertices.items).color)       / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(*obj->vertices.items), (void*)offsetof(GLutil_Vertex, color));
    glEnableVertexAttribArray(attrb_loc++);

    glVertexAttribPointer(attrb_loc, sizeof((*obj->vertices.items).tex_pos)     / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(*obj->vertices.items), (void*)offsetof(GLutil_Vertex, tex_pos));
    glEnableVertexAttribArray(attrb_loc++);

    glVertexAttribPointer(attrb_loc, sizeof((*obj->vertices.items).normal)      / sizeof(float), GL_FLOAT, GL_FALSE, sizeof(*obj->vertices.items), (void*)offsetof(GLutil_Vertex, normal));
    glEnableVertexAttribArray(attrb_loc++);

    // bind default buffer and vertex array as a cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    GLUTIL_CHECK_GL_ERROR();
    printf("       ╰─ OK.\n");
}


/*******************************************************************************
 *                                TEXTURES                                     *
 *******************************************************************************/


GLuint GLutil_texture_cubemap()
{
    printf("TODO: texture cubemap\n");
    return 0;
}

void GLutil_setup_fullscreen_buffer(GLutil* g)
{
    // Positions                  Texture Coords
    // (x, y)                     (x, y)               
    //                                                 
    // (-1, -1)     (+1, -1)       (0, 0)       (1, 0)
    //    +------------+             +------------+    
    //    |     |      |             |     |      |    
    //    |-----+------|             |-----+------|    
    //    |     |      |             |     |      |    
    //    +------------+             +------------+    
    // (-1, +1)     (+1, +1)       (0, 1)       (1, 1)
    float quad[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
        +1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, +1.0f,  0.0f, 1.0f,

        +1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, +1.0f,  0.0f, 1.0f,
        +1.0f, +1.0f,  1.0f, 1.0f,
    };

    // setup do quad vao
    GLuint quad_vbo;
    glGenVertexArrays(1, &g->fullscreen_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(g->fullscreen_vao);
    glBindBuffer( GL_ARRAY_BUFFER, quad_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW );
    GLuint attrib_ptr = 0;

    // pos, o primeiro par de floats por linha do quad
    glEnableVertexAttribArray(attrib_ptr);
    glVertexAttribPointer(attrib_ptr++, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),  (void*)0);

    // tex, o segunda par de floats por linha do quad
    glEnableVertexAttribArray(attrib_ptr);
    glVertexAttribPointer(attrib_ptr++, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),  (void*)(2 * sizeof(float)));
}


GLuint GLutil_generate_texture_2D() {
    GLuint texture_id;
    glGenTextures(1, &texture_id); GLUTIL_CHECK_GL_ERROR();
    glBindTexture(GL_TEXTURE_2D, texture_id); GLUTIL_CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); GLUTIL_CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); GLUTIL_CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); GLUTIL_CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GLUTIL_CHECK_GL_ERROR();
    return texture_id;
}


GLuint GLutil_texture_from_pixels_opt(int img_width, int img_height, unsigned char* pixels, GLenum texture_pixel_fmt, GLenum data_pixel_fmt)
{
    GLuint texture_id = GLutil_generate_texture_2D();
    glTexImage2D(
        GL_TEXTURE_2D, 0, texture_pixel_fmt,
        img_width, img_height,
        0, data_pixel_fmt, GL_UNSIGNED_BYTE,
        pixels
    ); GLUTIL_CHECK_GL_ERROR();
    glGenerateMipmap(GL_TEXTURE_2D); GLUTIL_CHECK_GL_ERROR();
    return texture_id;
}

GLuint GLutil_texture_from_pixels(int img_width, int img_height, unsigned char* pixels)
{
    return GLutil_texture_from_pixels_opt(img_width, img_height, pixels, GL_RGB, GL_RGB);
}

GLuint GLutil_texture_from_image(const char* image_file_name)
{
    printf("Loading Texture From Image '%s'\n", image_file_name);
    int img_width, img_height, n;
    unsigned char *data = stbi_load(image_file_name, &img_width, &img_height, &n, 0);

    if (data == NULL)  {
        PRINT_ERROR("stbi_load failed\n");
        exit(1);
    }

    GLuint texture_id;
    if ( n == 4 ) {
        texture_id = GLutil_texture_from_pixels_opt(img_width, img_height, data, GL_RGBA, GL_RGBA);
    } else if ( n == 3 ) {
        texture_id = GLutil_texture_from_pixels(img_width, img_height, data);
    } else {
        texture_id = -1;
        printf("       ╰─ NOT OK. Image have n = %d color channels. Loader only handler n = 3 or n = 4.\n", n);
        goto end;
    }
    printf("       ╰─ OK.\n");

end:
    stbi_image_free(data);
    return texture_id;
}


void GLutil_render_texture(GLutil* g, GLuint texture, int unit, float x, float y, float w, float h)
{
    shader_use(g, g->texture_shader);
    shader_add_int(*g->texture_shader, "the_texture", 0);
    shader_add_vec2f(*g->texture_shader, "window_size", (Vec2f) { g->winwidth, g->winheight });
    shader_add_vec4f(*g->texture_shader, "texture_pos_size", 
        (Vec4f) { 
            .x = x, .y = y,
            .z = w, .w = h, 
        }
    );

    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture  ( GL_TEXTURE_2D, texture );
    glBindVertexArray( g->fullscreen_vao );
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


/*******************************************************************************
 *                                MODELS                                       *
 *******************************************************************************/


GLutil_Model GLutil_make_model()
{
    GLutil_Model m = (GLutil_Model) {
        .scale       = 1.0f,  // @TODO: encontrar valores mais razoáveis de escala padrão lol
        .world_pos   = { 0.0f, 0.0f, 0.0f },
        .orientation = GLutil_unit_quaternion(), // { 1.0f, 0.0f, 0.0f, 0.0f } = no rotation
        .geometry    = NULL,
        .texture_id  = -1 // -1 == no texture
    };
    return m;
}

GLutil_Model GLutil_model_from_obj(GLutil_ObjData* obj)
{
    GLutil_Model m = GLutil_make_model();
    m.geometry = obj;
    return m;
}

#define DIFFUSE_TEXTURE_ID      GL_TEXTURE0
#define DIFFUSE_TEXTURE_INDEX   0
// #define SHADOWMAP_TEXTURE_ID    GL_TEXTURE1
// #define SHADOWMAP_TEXTURE_INDEX 1

void GLutil_render_model(GLutil* glutil, GLutil_Model* model)
{
    GLutil_ShaderProgram* shader = glutil->active_shader;

 // @NOTE: a ativação do shader deveria estar aqui? 
 //        ou render_model deveria sempre assumir que GLutil_shader_use(...) foi chamado antes?
    GLutil_shader_use(glutil, shader); 


    glBindVertexArray(model->geometry->vao_id); 
    CHECK_GL_ERROR();

    GLutil_shader_add_float(*shader, "t", GLutil_get_time_secs(glutil));

    GLutil_shader_add_mat3f(*shader, "model_mat", GLutil_model_matrix(glutil->camera));
    GLutil_shader_add_mat4f_transposed(*shader, "view_mat", GLutil_view_matrix(glutil->camera));
    GLutil_shader_add_mat4f_transposed(*shader, "proj_mat", GLutil_projection_matrix(glutil->camera, glutil->winwidth / (float)glutil->winheight));

    GLutil_shader_add_float(*shader, "scale", model->scale);
    GLutil_shader_add_vec3f(*shader, "world_pos", model->world_pos);
    GLutil_shader_add_quaternion(*shader, "orientation", model->orientation);

    GLutil_shader_add_vec3f(*shader, "camera_pos", glutil->camera.pos);

    GLutil_shader_add_int(*shader, "the_texture", DIFFUSE_TEXTURE_INDEX);


    GLutil_shader_add_int(*shader, "lights_count", glutil->lights.len);
    for (size_t i = 0; i < glutil->lights.len; ++i) {
        char posstr[50];
        sprintf(posstr, "lights[%ld].pos", i);
        GLutil_shader_add_vec3f(*shader, posstr, glutil->lights.items[i].pos);

        char clrstr[50];
        sprintf(clrstr, "lights[%ld].clr", i);
        GLutil_shader_add_glcolor(*shader, clrstr, glutil->lights.items[i].clr);

        char shinstr[50];
        sprintf(shinstr, "lights[%ld].shine", i);
        GLutil_shader_add_float(*shader, shinstr, glutil->lights.items[i].shine);

        char intenstr[50];
        sprintf(intenstr, "lights[%ld].intensity", i);
        GLutil_shader_add_float(*shader, intenstr, glutil->lights.items[i].intensity);


        GLuint shadowmap_texture_index = i + 1;

        char shadow_map_str[50];
        sprintf(shadow_map_str, "shadow_maps[%ld]", i);
        GLutil_shader_add_int(*shader, shadow_map_str, shadowmap_texture_index);

        glActiveTexture( GL_TEXTURE0 + shadowmap_texture_index );
        glBindTexture( GL_TEXTURE_2D, glutil->lights.items[i].shadow_map );


        GLutil_Mat4x4f light_mat = GLutil_light_space_matrix(glutil->lights.items[i]);
        char light_mat_str[50];
        sprintf(light_mat_str, "lights[%ld].light_mat", i);
        GLutil_shader_add_mat4f(*shader, light_mat_str, light_mat);

    }


    if ( model->texture_id == (GLuint)-1 ) {
        GLutil_shader_add_int(*shader, "has_texture", false);
    } else {
        GLutil_shader_add_int(*shader, "has_texture", true);
        glActiveTexture(DIFFUSE_TEXTURE_ID); CHECK_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, model->texture_id); CHECK_GL_ERROR();
    }

    glDrawElements(GL_TRIANGLES, model->geometry->faces.len, GL_UNSIGNED_INT, 0); CHECK_GL_ERROR();
    // printf("[DRAW] Error: %s\n", glErrorNames[glGetError()]);

}
// ] */


void GLutil_print_framebuffer_status(GLenum status) 
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

#endif // GLUTIL_H_


/*
 *
 * Não sei onde colocar isso, mas é legal, e talvez faça algo isso eventualmente
 *
 */

// Render Buffer for rendering stuff from the framebuffer
// informações a respeito do depth buffer, e portanto não precisa renderizar nada
// renderbuffer não é necessário para o shadowmap, pois a gente só quer

/*
GLuint renderbuffer;
glGenRenderbuffers(1, &renderbuffer); CHECK_GL_ERROR();

glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer); CHECK_GL_ERROR();
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINWIDTH, WINHEIGHT); CHECK_GL_ERROR();
glBindRenderbuffer(GL_RENDERBUFFER, 0); CHECK_GL_ERROR(); // default renderbuffer

glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
    GL_RENDERBUFFER, renderbuffer
); CHECK_GL_ERROR();

GLenum fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
while ( fb_status != GL_FRAMEBUFFER_COMPLETE) {
    printf("[ERROR] Framebuffer didn't complete.\n");
    print_framebuffer_status(fb_status);
    fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
}


// Quad for rendering two triangles that take the whole screen

    float quad_verts[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
    };

    // setup do quad vao
    GLuint quad_vao, quad_vbo;
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    glBindBuffer( GL_ARRAY_BUFFER, quad_vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW );
    GLuint attrib_ptr = 0;
    // pos, o primeiro par de floats por linha de quad_verts
    glEnableVertexAttribArray(attrib_ptr);
    glVertexAttribPointer(attrib_ptr++, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),  (void*)0);
    // tex, o segunda par de floats por linha de quad_verts
    glEnableVertexAttribArray(attrib_ptr);
    glVertexAttribPointer(attrib_ptr++, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),  (void*)(2 * sizeof(float)));
}
*/




/*******************************************************************************
 *                                                                             *
 *                       LINCENSES / LICENÇAS                                  *
 *                                                                             *
 ******************************************************************************/


/************
 * RGFW     *
 ***********/

/* 
* Most of the OpenGl initialization code was taken from:
* @FROM: https://github.com/ColleagueRiley/OpenGL-Context-Creation
* made by the author of
* https://github.com/ColleagueRiley/RGFW 
* with license:
*/

/*
  RGFW 2.0.0-dev

Copyright (C) 2022-26 Riley Mabb (@ColleagueRiley)

libpng license

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

*/





/************
 * RAYLIB   *
 ***********/

/* Some of the algebra linear and color conversion (RGB <-> HSV) code was copied
 * from raylib source code.
 * https://www.raylib.com/
 * https://github.com/raysan5/raylib
 * with License:
*/

/*
Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)

This software is provided "as-is", without any express or implied warranty. In no event 
will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim that you 
  wrote the original software. If you use this software in a product, an acknowledgment 
  in the product documentation would be appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be misrepresented
  as being the original software.

  3. This notice may not be removed or altered from any source distribution.
*/





/*************
 * STB_IMAGE *
 *************/

/* stb_image ( https://github.com/nothings/stb/blob/master/stb_image.h ) 
 * is used for loading image data as an array os pixels
 * stb_image offers two licenses. Both are presented here:
 */

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/

