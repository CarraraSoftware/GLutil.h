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
