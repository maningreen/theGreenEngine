SRC = src/*.cpp
BUILDDIR = build/
OUT = engine
LDFLAGS = 
CC = g++
RAYLIB ?= ./external/raylib-5.5/src/
RAYLIBFLAGS = -I $(RAYLIB) -L$(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
ENGINEOUT = build/*.o

game:
	$(CC) $(SRC) $(ENGINEOUT) $(LDFLAGS) $(RAYLIBFLAGS) -o $(BUILDDIR)$(OUT) -L$(BUILDDIR) -lengine

all:
	make engine
	make program

engine: # -c makes it not link
	g++ -c src/engine/entity.cpp -o $(BUILDDIR)entity.o
	g++ -c src/engine/core.cpp -o $(BUILDDIR)core.o
	g++ -c src/engine/main.cpp -o $(BUILDDIR)main.o
	ar rcs $(BUILDDIR)libengine.a $(ENGINEOUT)
	rm $(BUILDDIR)*.o

clean:
	rm -rf $(BUILDDIR)*
