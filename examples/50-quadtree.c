#include <assert.h>
#include <sys/random.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define GLUTIL_IMPLEMENTATION
#include "../glutil.h"


#define WW 1500
#define WH 1200

#define PARTICLE_COUNT    250
#define PARTICLE_MIN_RAD  5
#define PARTICLE_MAX_RAD  5
#define PARTICLE_MAX_VEL  10.0f // pixels per second
#define PARTICLE_MIN_VEL -10.0f // pixels per second 


#define QUAD_CAPACITY 3

typedef struct {
    Vec2f pos; // 8 bytes
    Vec2f vel; // 8 bytes
    Vec2f acc; // 8 bytes
    int   rad; // 4 bytes
    Color clr; // 4 bytes
} Particle;


enum {
    TOP_RIGHT,
    TOP_LEFT,
    BOT_LEFT,
    BOT_RIGHT,
    QUAD_CHILDREN_COUNT,
} QuadrantDirs;

typedef struct Quadrant Quadrant;

struct Quadrant { 
    Vec2f pos;
    Vec2f wh;

    // um quadrante pode
    // ter partículas
    int particles[QUAD_CAPACITY];
    int particles_count;
    // ou ter subregiões
    Quadrant* children;
};


void quad_insert(Quadrant* quad, int particle_idx);
void quad_print(Quadrant* quad, int lvl);


Particle particles[PARTICLE_COUNT] = {0};


// @TODO: move this to glutil.h
void rect_outline(GLutil_Vec2f topleft, float w, float h, GLutil_Color clr)
{
    glColor4ub( clr.red, clr.green, clr.blue, clr.alpha );
    glLineWidth( 2.0f );
    glBegin( GL_LINE_LOOP );
    glVertex2f( topleft.x,      topleft.y     );
    glVertex2f( topleft.x,      topleft.y + h );
    glVertex2f( topleft.x + w , topleft.y + h );
    glVertex2f( topleft.x + w , topleft.y     );
    glEnd();
}

void particle_check_collisions(uint64_t dt, int* ps, int idx, int ps_size)
{

    Particle* self = &particles[ps[idx]];


    for (int i = idx + 1; i < ps_size; ++i) {
        if (i == idx) continue;

        Particle* other = &particles[ps[i]];

        // norm =>   self <---------- other
        Vec2f norm  = GLutil_Vec2f_sub(self->pos, other->pos);
        size_t dist = GLutil_Vec2f_length(norm);

        if (dist > self->rad + other->rad) continue;

        float theta  = atan2f(norm.y, norm.x);
        float sine   = sinf(theta);
        float cosine = cosf(theta);
        
        Vec2f pv = self->vel;
        Vec2f pv_transf = (Vec2f) {
            .x = + pv.x * cosine + pv.y * sine,
            .y = - pv.x * sine   + pv.y * cosine,
        };

        Vec2f ov = other->vel;
        Vec2f ov_transf = (Vec2f) {
            .x = + ov.x * cosine + ov.y * sine,
            .y = - ov.x * sine   + ov.y * cosine,
        };


        // convert back to regular x and y
        sine   = -sine;

        Vec2f p_after = (Vec2f) {
            .x = ov_transf.x,
            .y = pv_transf.y, 
        };
        self->vel.x = p_after.x * cosine + p_after.y * sine;
        self->vel.y = - p_after.x * sine + p_after.y * cosine;

        Vec2f o_after = (Vec2f) {
            .x = pv_transf.x,
            .y = ov_transf.y,
        };
        other->vel.x = o_after.x * cosine + o_after.y * sine;
        other->vel.y = - o_after.x * sine   + o_after.y * cosine;

        float mag = (self->rad + other->rad - dist) / 2.0f ;
        Vec2f shift = norm;
        float cur_mag = GLutil_Vec2f_length(shift);
        float factor = mag / cur_mag;
        shift.x *= factor;
        shift.y *= factor;

        self->pos  = GLutil_Vec2f_add(self->pos, shift);
        other->pos = GLutil_Vec2f_sub(other->pos, shift);
    }
}

void particle_update(uint64_t dt, int* ps, int idx, int ps_size)
{
    Particle* p = &particles[ps[idx]];


    float dt_secs = (float) (float)dt / (float)(1000 * 1000 * 1000);

    p->acc.x = 0;
    p->acc.y = 0;

    // p->acc.y += PARTICLE_GRAVITY * dt_secs;

    p->vel.x += p->acc.x * dt_secs;
    p->vel.y += p->acc.y * dt_secs;

    int new_x = p->pos.x + p->vel.x;
    if (new_x + p->rad < WW && new_x - p->rad >= 0) {
        p->pos.x += p->vel.x;
    } else {
        if (new_x + p->rad >= WW) {
            p->pos.x = WW - p->rad - 1;
        } else if (new_x - p->rad < 0) {
            p->pos.x = p->rad;
        }
        p->vel.x *= -1;
    }

    int new_y = p->pos.y + p->vel.y;
    if (new_y + p->rad < WH && new_y - p->rad >= 0) {
        p->pos.y += p->vel.y;
    } else {
        if (new_y + p->rad >= WH) {
            p->pos.y = WH - p->rad - 1;
        } else if (new_y - p->rad < 0) {
            p->pos.y = p->rad;
        }
        p->vel.y *= -1;
    }

    particle_check_collisions(dt, ps, idx, ps_size);

    return;
}


void particle_draw(Particle* p, uint64_t dt)
{
    (void)dt;
    circle(p->pos, p->rad, p->clr);
}


uint64_t get_time()
{
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC, &ts );
    long int secs = ts.tv_sec;
    long int nanos = ts.tv_nsec;
    uint64_t t = (secs * 1000000000) + nanos;
    return t;
}

void quad_subdivide(Quadrant* quad, int particle_idx)
{
    assert(quad->children == NULL);
    quad->children = (Quadrant*)malloc(4 * sizeof(Quadrant));
    quad->children[TOP_RIGHT] = (Quadrant) {
        .pos = (Vec2f) {
            .x = quad->pos.x + (quad->wh.x / 2.0f),
            .y = quad->pos.y,
        },
        .wh  = (Vec2f) {
            .x = (quad->wh.x / 2.0f),
            .y = (quad->wh.y / 2.0f),
        },
        .children = NULL,
        .particles = {0},
        .particles_count = 0,
    };

    quad->children[TOP_LEFT] = (Quadrant) {
        .pos = (Vec2f) {
            .x = quad->pos.x,
            .y = quad->pos.y,
        },
        .wh  = (Vec2f) {
            .x = (quad->wh.x / 2.0f),
            .y = (quad->wh.y / 2.0f),
        },
        .children = NULL,
        .particles = {0},
        .particles_count = 0,
    };

    quad->children[BOT_LEFT] = (Quadrant) {
        .pos = (Vec2f) {
            .x = quad->pos.x + (quad->wh.x / 2.0f),
            .y = quad->pos.y + (quad->wh.y / 2.0f),
        },
        .wh  = (Vec2f) {
            .x = (quad->wh.x / 2.0f),
            .y = (quad->wh.y / 2.0f),
        },
        .children = NULL,
        .particles = {0},
        .particles_count = 0,
    };

    quad->children[BOT_RIGHT] = (Quadrant) {
        .pos = (Vec2f) {
            .x = quad->pos.x,
            .y = quad->pos.y + (quad->wh.y / 2.0f),
        },
        .wh = (Vec2f) {
            .x = (quad->wh.x / 2.0f),
            .y = (quad->wh.y / 2.0f),
        },      
        .children = NULL,
        .particles = {0},
        .particles_count = 0,
    };


    for (int i = 0; i < quad->particles_count; ++i)
        for (int j = 0; j < QUAD_CHILDREN_COUNT; ++j)
            quad_insert(&quad->children[j], quad->particles[i]);


    for (int j = 0; j < QUAD_CHILDREN_COUNT; ++j)
        quad_insert(&quad->children[j], particle_idx);


    quad->particles_count = -1;
}

bool quad_contains(Quadrant* quad, int particle_idx)
{

    Particle p = particles[particle_idx];

    float ax = quad->pos.x;
    float ay = quad->pos.y;
    float bx = quad->pos.x + quad->wh.x;
    float by = quad->pos.y + quad->wh.y;

    float dx;
    float x = p.pos.x;
    float dy;
    float y = p.pos.y;

    if      (x <= ax)            dx = ax - x; 
    else if (ax < x && x < bx)   dx = 0;      
    else if (x >= bx)            dx = x - bx; 

    if      (y <= ay)            dy = ay - y; 
    else if (ay < y && y < by)   dy = 0;      
    else if (y >= by)            dy = y - by; 

    return dx*dx + dy*dy < p.rad*p.rad;
}


void quad_destroy(Quadrant* quad)
{
    if (quad->children != NULL) {
        for (int i = 0; i < QUAD_CHILDREN_COUNT; ++i)
            quad_destroy(&quad->children[i]);
        free(quad->children);
        quad->children = NULL;
    }
    quad->particles_count = 0;
}


void quad_insert(Quadrant* quad, int particle_idx)
{
    Particle p = particles[particle_idx];

    // a partícula não faz parte desse quadrante
    if (!quad_contains(quad, particle_idx)) 
        return;

    // a partícula faz parte e a gente já subdividiu antes
    if (quad->particles_count == -1 && quad->children != NULL) {
        for (int i = 0; i < QUAD_CHILDREN_COUNT; ++i)
            quad_insert(&quad->children[i], particle_idx);
        return;
    }

    // a partícula faz parte e a gente pode inserir no array de partículas
    if (quad->particles_count < QUAD_CAPACITY) {
        quad->particles[quad->particles_count] = particle_idx;
        quad->particles_count++;
        return;
    }

    // a partícula faz parte e a gente ainda não subdividiu
    quad_subdivide(quad, particle_idx);
}



void quad_update_particles(Quadrant* quad, uint64_t dt)
{
    if (quad->particles_count == -1 && quad->children != NULL) {
        for (int i = 0; i < QUAD_CHILDREN_COUNT; ++i)
            quad_update_particles(&quad->children[i], dt);
        return;
    }

    for (int i = 0; i < quad->particles_count; ++i)
        particle_update(dt, quad->particles, i, quad->particles_count);

    return;
}

void quad_draw_particles(Quadrant* quad, uint64_t dt)
{
    rect_outline(quad->pos, quad->wh.x, quad->wh.y, (Color) { 255, 255, 255, 255 });

    if (quad->particles_count == -1 && quad->children != NULL) {
        for (int i = 0; i < QUAD_CHILDREN_COUNT; ++i)
            quad_draw_particles(&quad->children[i], dt);
        return;
    }

    for (int i = 0; i < quad->particles_count; ++i)
        particle_draw(&particles[quad->particles[i]], dt);

    return;
}

void quad_print(Quadrant* quad, int lvl)
{
    for (int i = 0; i < lvl; ++i) printf("   ");
    printf("{ x = %f, y = %f, w = %f, h = %f, part_count = %d }\n",
            quad->pos.x, quad->pos.y, quad->wh.x, quad->wh.y, quad->particles_count);
    if (quad->children != NULL) 
        for (int i = 0; i < QUAD_CHILDREN_COUNT; ++i)
            quad_print(&quad->children[i], lvl + 1);
}


void particles_init()
{
    for (int i = 0; i < PARTICLE_COUNT; ++i) {
        Particle* p = &particles[i];
    
        p->pos = (Vec2f) {
            .x = RAND(0, WW - 1),
            .y = RAND(0, WH - 1),
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
}



int main(void)
{
    srand(get_time_nanos());
    GLutil glutil = {0};
    init(&glutil, WW, WH);


    Quadrant root = {0};                     // o primeiro quadrante 
    root.pos = (Vec2f) { .x = 0,  .y = 0, }; // começa no topo esquerda,
    root.wh  = (Vec2f) { .x = WW, .y = WH }; // ocupa a janela toda
    root.children = NULL;                    // e não tem subdivisões

    particles_init();

    set_target_fps(&glutil, 60.0f);
    for (;;) {
        uint64_t dt = begin_frame(&glutil);
            clear_bg(BLACK);

            for (int i = 0; i < PARTICLE_COUNT; ++i) quad_insert(&root, i);
            quad_update_particles(&root, dt);
            quad_draw_particles(&root, dt);
            quad_destroy(&root);

        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
}
