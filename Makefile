# SRC
SRC = $(wildcard src/*.cpp)
ENGINESRC = $(wildcard src/engine/*.cpp)

# OBJECTS
OBJECTS = $(SRC:src/%.cpp=$(BUILDDIR)%.o)
ENGINEOBJS = $(ENGINESRC:src/engine/%.cpp=$(BUILDDIR)%.o)
ENGINEOUT = build/libengine.a

# define misc
CC = g++
RAYLIB ?= ./external/raylib-5.5/src/

# flags
ENGINEFLAGS = $(ENGINEOUT) -L$(BUILDDIR) -lengine
RAYLIBFLAGS = -I $(RAYLIB) -L$(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LDFLAGS =

# target
OUT = engine
BUILDDIR = build/

game: $(ENGINEOUT)
	$(CC) $(SRC) $(LDFLAGS) $(RAYLIBFLAGS) -o $(BUILDDIR)$(OUT) $(ENGINEFLAGS)

engine: $(ENGINEOUT)

$(ENGINEOUT): $(ENGINEOBJS) # -c makes it not link
	ar rcs $(ENGINEOUT) $(ENGINEOBJS)

$(BUILDDIR)%.o: src/engine/%.cpp
	$(CC) $(LDFLAGS) $(RAYLIBFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)
	mkdir $(BUILDDIR)
