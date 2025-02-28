SRC = src/*.cpp
BUILDDIR = build/
OUT = engine
LDFLAGS = -lraylib
CC = g++
RAYLIB ?= ./external/raylib-5.5/src/
RAYLIBFLAGS = -I $(RAYLIB) -L$(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

program:
	$(CC) $(SRC) $(LDFLAGS) $(RAYLIBFLAGS) -o $(BUILDDIR)$(OUT)
