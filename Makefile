OUT_DIR=bin/
LINKER_FLAGS=-lX11 -lGL -lm
F_FLAGS=-flto=auto -fno-fat-lto-objects -Wall -Wextra -pedantic -Wno-unused-parameter -Wstrict-prototypes -std=gnu99 -Wshadow -Wconversion -Wvla -Wdouble-promotion -Wmissing-noreturn -Wmissing-format-attribute -Wmissing-prototypes -Wno-unused-function -fsigned-char -fstack-protector-strong -Wno-conversion -fno-common -Wno-unused-result -Wimplicit-fallthrough -fdiagnostics-color=auto -Wno-free-nonheap-object


dvd:
	gcc -o $(OUT_DIR)dvd examples/00-circle_dvd.c $(LINKER_FLAGS)
	$(OUT_DIR)dvd

clock:
	gcc -o $(OUT_DIR)clock examples/10-clock.c $(LINKER_FLAGS)
	$(OUT_DIR)clock

texture:
	gcc -o $(OUT_DIR)tex examples/15-render_texture.c $(LINKER_FLAGS)
	$(OUT_DIR)tex

particles:
	gcc -o $(OUT_DIR)particles examples/20-particles.c  $(LINKER_FLAGS)
	$(OUT_DIR)particles

quadtree:
	gcc -o $(OUT_DIR)quad examples/50-quadtree.c $(LINKER_FLAGS)
	$(OUT_DIR)quad

cube:
	gcc -o $(OUT_DIR)cube examples/100-textured_cube/texcube.c $(LINKER_FLAGS)
	$(OUT_DIR)cube

ping-pong:
	gcc -o $(OUT_DIR)ping-pong examples/200-ping-pong/pong.c $(LINKER_FLAGS)
	$(OUT_DIR)ping-pong

cubes:
	gcc -o $(OUT_DIR)cubes examples/150-cubes_and_camera/cubes_camera.c $(LINKER_FLAGS)
	$(OUT_DIR)cubes

shadow:
	gcc -ggdb -o $(OUT_DIR)shadow examples/250-shadow/shadow.c $(LINKER_FLAGS)
	$(OUT_DIR)shadow


