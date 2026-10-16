# Descrição

apenas alguns experimentos com OpenGL.
atualmente feito especificamente pra rodar em sistemas linux.
os requisitos são:
- OpenGL
- X11

não sei quais versões necessárias, mas honestamente, qualquer distro linux provavelmente satisfaz os requisitos.

# Como Usar
na pasta 'examples' tem uns programas mais interessantes feitos com o GLutil.h,
mas o básico é:
```c
// arquivo main.c

#define GLUTIL_IMPLEMENTATION // inclui as implementações de todas as funções
#include "glutil.h"

int main(void)
    GLutil glutil = {0};
    init(&glutil, 800, 600);
    set_target_fps(&glutil, 60.0f);

    for (;;) {
        uint64_t dt = begin_frame(&glutil);
            clear_bg(BLACK);
            circle(
                (Vec2f) { 100.0f, 100.0f },  // centro do círculo
                50,                          // raio do círculo
                COLOR( 255, 255, 255 )       // cor do círculo
            );
        end_frame(&glutil);
    }

    destroy(&glutil);
    return 0;
{

```

e então compilaria e rodaria com:
```bash
gcc -o main main.c -lX11 -lGL -lm
./main
```




# Referências

### OpenGL
- [OpenGL 4.6 Specs](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf)
- [OpenGL Functions Search Engine](https://docs.gl/)
- [OpenGL Hello World (usa o GLUT)](https://github.com/mrotaru/OpenGL-linux)
- [OpenGL Context Creation (do criador do RGFW)](https://github.com/ColleagueRiley/OpenGL-Context-Creation) 
- [RGFW - lib initializer OpenGL ](https://github.com/ColleagueRiley/OpenGL-Context-Creation)
- [Comparação de Arquitetura do Wayland e X11](https://wayland.freedesktop.org/architecture.html)
- [EGL Overview](https://www.khronos.org/egl/)
- [Linux Graphics Stack](https://thumb.wikimedia.org/wikipedia/commons/thumb/c/c2/Linux_Graphics_Stack_2013.svg/3840px-Linux_Graphics_Stack_2013.svg.png)
    > By Shmuel Csaba Otto Traian, CC BY-SA 3.0, https://commons.wikimedia.org/w/index.php?curid=27858390

### Collisions
- [Wikipedia - Elastic Collisions](https://en.wikipedia.org/wiki/Elastic_collision)

### QuadTree
- [Wikipedia - QuadTree](https://en.wikipedia.org/wiki/Quadtree)
- [First(?) Article Introducing QuadTree](https://www.researchgate.net/publication/220197855_Quad_Trees_A_Data_Structure_for_Retrieval_on_Composite_Keys)


## Commandos úteis
- glxinfo  - printa informações sobre versões de drivers e libs 
             instaladas no sistema
- glxgears - testa a implementação do OpenGL em um sistema 
             abrindo uma janela com uma animação de engrenagens
             


