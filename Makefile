SRC = src/*.cpp
BUILDDIR = build/
OUT = engine
LDFLAGS = 
CC = g++
RAYLIB ?= ./external/raylib-5.5/src/
RAYLIBFLAGS = -I $(RAYLIB) -L$(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
ENGINEOUT = build/*.o

program:
	$(CC) $(SRC) $(ENGINEOUT) $(LDFLAGS) $(RAYLIBFLAGS) -o $(BUILDDIR)$(OUT) -L$(BUILDDIR) -lengine

engine: # -c makes it not link
	g++ -c src/engine/entity.cpp -o build/entity.o
	g++ -c src/engine/core.cpp -o build/core.o
	g++ -c src/engine/main.cpp -o build/main.o
	ar rcs build/libengine.a $(ENGINEOUT)

clean:
	rm -rf $(BUILDDIR)*
