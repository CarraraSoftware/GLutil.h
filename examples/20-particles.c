#include <sys/random.h>
#include <stdbool.h>

#define GLUTIL_IMPLEMENTATION
#include "../glutil.h"

#define WINDOW_WIDTH  1500
#define WINDOW_HEIGHT 1200

#define PARTICLE_COUNT    50
#define PARTICLE_MIN_RAD  20
#define PARTICLE_MAX_RAD  40
#define PARTICLE_MAX_VEL  10.0f // pixels per second
#define PARTICLE_MIN_VEL -10.0f // pixels per second 

// #define PARTICLE_GRAVITY 3000.0f
#define PARTICLE_GRAVITY 0.0f

typedef struct {
    Vec2f pos; // 8 bytes
    Vec2f vel; // 8 bytes
    Vec2f acc; // 8 bytes
    int   rad; // 4 bytes
    Color clr; // 4 bytes
} Particle;

Particle particles[PARTICLE_COUNT] = {0};

void particle_check_collisions(Particle* self, uint64_t dt, int idx)
{

    for (int i = idx + 1; i < PARTICLE_COUNT; ++i) {
        if (i == idx) continue;

        Particle* other = &particles[i];

        // 'norm' é o vetor que o centro da partícula 'other' até
        // o centro da partícula 'self'. esse vetor é utilizado pra definir
        // a direção Normal da colisão. essa direção é a única direção em que
        // de fato há troca de momentum.
        Vec2f norm  = Vec2f_sub(self->pos, other->pos);
        size_t dist = Vec2f_length(norm);

        // se não há intersecção entre as partículas, ignorar
        if (dist > self->rad + other->rad) continue;

        // theta é o ângulo entre o vetor 'norm' e o eixo horizontal (x).
        // esse ângulo é utilizado pra projetar o vetor velocidade de cada
        // partícula na direção normal da colisão e na direção tangencial
        float theta  = atan2f(norm.y, norm.x);
        float sine   = sinf(theta);
        float cosine = cosf(theta);
        
        Vec2f pv = self->vel;
        Vec2f pv_transf = (Vec2f) {
            .x = + pv.x * cosine + pv.y * sine,
            .y = - pv.x * sine   + pv.y * cosine,
        }; // velocidade da partícula 'self' projetados na normal e tangente

        Vec2f ov = other->vel;
        Vec2f ov_transf = (Vec2f) {
            .x = + ov.x * cosine + ov.y * sine,
            .y = - ov.x * sine   + ov.y * cosine,
        }; // velocidade da partícula 'other' projetados na normal e tangente


        // inverter o seno, efetivamente é equivalente a inverter o ângulo theta,
        // de maneira a permitir a projeção inversa da anterior.
        // isto é, projetar as direções normal e tangete à colisão
        // de volta aos eixos originais x e y.
        sine   = -sine;

        Vec2f p_after = (Vec2f) {
            .x = ov_transf.x,  // componente normal da velocidade da partícula 'other', essa troca representa a colisão.
            .y = pv_transf.y,  
        };
        // projeta de volta nos eixos x e y
        self->vel.x = p_after.x * cosine + p_after.y * sine;
        self->vel.y = - p_after.x * sine + p_after.y * cosine;

        Vec2f o_after = (Vec2f) {
            .x = pv_transf.x, // componente normal da velocidade da partícula 'self', essa troca representa a colisão.
            .y = ov_transf.y,
        };
        // projeta de volta nos eixos x e y
        other->vel.x = o_after.x * cosine + o_after.y * sine;
        other->vel.y = - o_after.x * sine   + o_after.y * cosine;


        // ajusta as posições das partículas pra evitar que a colisão seja
        // detectada novamente no próximo frame
        float mag = (self->rad + other->rad - dist) / 2.0f ;
        Vec2f shift = norm;
        float cur_mag = Vec2f_length(shift);
        float factor = mag / cur_mag;
        shift.x *= factor;
        shift.y *= factor;

        self->pos  = Vec2f_add(self->pos, shift);
        other->pos = Vec2f_sub(other->pos, shift);
    }
}

void particle_update(Particle* p, uint64_t dt, int idx)
{
    float dt_secs = (float) (float)dt / (float)(1000 * 1000 * 1000);

    p->acc.x = 0;
    p->acc.y = 0;

    p->acc.y += PARTICLE_GRAVITY * dt_secs;

    p->vel.x += p->acc.x * dt_secs;
    p->vel.y += p->acc.y * dt_secs;

    int new_x = p->pos.x + p->vel.x;
    if (new_x + p->rad < WINDOW_WIDTH && new_x - p->rad >= 0) {
        p->pos.x += p->vel.x;
    } else {
        if (new_x + p->rad >= WINDOW_WIDTH) {
            p->pos.x = WINDOW_WIDTH - p->rad - 1;
        } else if (new_x - p->rad < 0) {
            p->pos.x = p->rad;
        }
        p->vel.x *= -1;
    }

    int new_y = p->pos.y + p->vel.y;
    if (new_y + p->rad < WINDOW_HEIGHT && new_y - p->rad >= 0) {
        p->pos.y += p->vel.y;
    } else {
        if (new_y + p->rad >= WINDOW_HEIGHT) {
            p->pos.y = WINDOW_HEIGHT - p->rad - 1;
        } else if (new_y - p->rad < 0) {
            p->pos.y = p->rad;
        }
        p->vel.y *= -1;
    }

    particle_check_collisions(p, dt, idx);

    return;
}


void particle_draw(Particle p)
{
     circle( p.pos, p.rad, p.clr );
     return;
     Vec2f a = p.pos;
     Vec2f b = (Vec2f) {
         .x = a.x + 20 * p.vel.x * 2,
         .y = b.x + 20 * p.vel.y * 2,
     };
     line( a, b, p.clr, 20.0f );
}

void particle_print(Particle p)
{
    printf(
        " { p.x = %f, p.y = %f,\n "
        "   v.x = %f, v.y = %f,\n "
        "   rad = %d\n"
        "   red = %d, green = %d, blue = %d }\n\n",
        p.pos.x, p.pos.y, p.vel.x, p.vel.y, p.rad, p.clr.red, p.clr.green, p.clr.blue
    );
}



int main(void)
{
    uint64_t t = get_time_nanos();
    srand(t);

    GLutil glutil = {0};
    init(&glutil, WINDOW_WIDTH, WINDOW_HEIGHT);
         

    for (int i = 0; i < PARTICLE_COUNT; ++i) {
        Particle* p = &particles[i];
    
        p->pos = (Vec2f) {
            .x = RAND(0, WINDOW_WIDTH - 1),
            .y = RAND(0, WINDOW_HEIGHT - 1),
        };
        p->vel = (Vec2f) {
            .x = RAND(PARTICLE_MIN_VEL, PARTICLE_MAX_VEL),
            .y = RAND(PARTICLE_MIN_VEL, PARTICLE_MAX_VEL),
        };
        p->acc = (Vec2f) {
            .x = 0,
            .y = 0,
        };
        p->rad = (int)RAND(PARTICLE_MIN_RAD, PARTICLE_MAX_RAD);
        p->clr = (Color) {
            .red   = (int)RAND(0, 255),
            .green = (int)RAND(0, 255),
            .blue  = (int)RAND(0, 255),
            .alpha = (int)255,
        };
    }

    float theta = 0;
    for (;;) {
        uint64_t dt = begin_frame(&glutil);

        clear_bg(BLACK);

        for (int i = 0; i < PARTICLE_COUNT; ++i) 
            particle_update( &particles[i], dt, i );

        for (int i = 0; i < PARTICLE_COUNT; ++i) 
            particle_draw( particles[i] );


        end_frame(&glutil);
    }

    destroy(&glutil);

    return 0;
}
