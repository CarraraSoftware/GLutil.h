#include <stdio.h>

#define GLUTIL_IMPLEMENTATION
#include "../glutil.h"

#define WIDTH  1600
#define HEIGHT 900


int main(void)
{
    GLutil g = {};
    init(&g, WIDTH, HEIGHT);

    #define img_width WIDTH / 2
    #define img_height HEIGHT / 2
    unsigned char pixels[3 * img_width * img_height] = {};
    for ( int j = 0; j < img_height; ++j ) {
        for ( int i = 0; i < img_width; ++i ) {
            int idx = 3*(j * img_width + i);
            pixels[idx]   = 0x00;
            pixels[idx+1] = 0x00;
            pixels[idx+2] = 0xFF;
        }
    }

    GLuint texture = texture_from_pixels( img_width, img_height, pixels );

    // Esse exemplo também funciona se a texture for carregada de uma imagem:
    // GLuint texture = texture_from_image( "SOME_RANDOM_IMAGE.png" );

    const int fac = 30;

    // A posição em pixels onde o topo esquerda da textura estará na tela:
    float  texture_x = 0;
    float  texture_y = 0;

    // O tamanho em pixels com que a texture vai ser renderizada na tela:
    float  texture_w = 16.0f * (float)fac; 
    float  texture_h =  9.0f * (float)fac;

    while (true) {
        uint64_t dt = begin_frame(&g);
            float t = get_time_secs(&g);
            if ( g.input.space_bar.down ) shader_reload(g.texture_shader);

            clear_bg(BLACK);
            glDisable(GL_DEPTH_TEST);

            // Um controlezinho pra mover a textura pra lá e pra cá só pra fazer uma graça
            {
                const float step = 5.0f;
                if ( g.input.arrow_right.pressed ) {
                    texture_x += step;
                    if ( texture_x >= WIDTH ) texture_x = 0;
                }

                if ( g.input.arrow_left.pressed ) {
                    texture_x -= step;
                    if ( texture_x < 0 ) texture_x = WIDTH - 1;
                }

                if ( g.input.arrow_down.pressed ) {
                    texture_y += step;
                    if ( texture_y >= HEIGHT ) texture_y = 0;
                }

                if ( g.input.arrow_up.pressed ) {
                    texture_y -= step;
                    if ( texture_y < 0 ) texture_y = HEIGHT - 1;
                }
            }

            // @NOTE: O terceiro argumento é a texture_unit associada a textura.
            //         Isto é, qual unidade de textura interna do OpenGL está associada
            //        com a textura que deve ser renderizada.
            //         Como essa é uma função dedicada exclusivamente a renderizar 
            //        uma única textura, acho que não vai existir uma situação 
            //        em que seja necessário mais de uma texture_unit.
            //         Mas fiquei na dúvida e decidi manter isso como argumento, 
            //        caso decida expandir para renderizar várias texturas.
            render_texture(&g, texture, 0, texture_x, texture_y, texture_w, texture_h);

        end_frame(&g);
    }

    destroy(&g);
    return 0;
}
