OUT_DIR=bin/
LINKER_FLAGS=-lX11 -lGL -lm
dvd:
	gcc -o $(OUT_DIR)dvd examples/00-circle_dvd.c $(LINKER_FLAGS)
	$(OUT_DIR)dvd

clock:
	gcc -o $(OUT_DIR)clock examples/10-clock.c $(LINKER_FLAGS)
	$(OUT_DIR)clock

particles:
	gcc -o $(OUT_DIR)particles examples/20-particles.c  $(LINKER_FLAGS)
	$(OUT_DIR)particles

quadtree:
	gcc -o $(OUT_DIR)quad examples/50-quadtree.c $(LINKER_FLAGS)
	$(OUT_DIR)quad

cube:
	gcc -o $(OUT_DIR)cube examples/100-textured_cube/main.c $(LINKER_FLAGS)
	$(OUT_DIR)cube

ping-pong:
	gcc -o $(OUT_DIR)ping-pong examples/200-ping-pong/main.c $(LINKER_FLAGS)
	$(OUT_DIR)ping-pong

cubes:
	gcc -o $(OUT_DIR)cubes examples/150-cubes_and_camera/main.c $(LINKER_FLAGS)
	$(OUT_DIR)cubes


