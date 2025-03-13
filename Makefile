# SRC
SRC = $(wildcard src/*.cpp)
ENGINESRC = $(wildcard src/engine/*.cpp)

# OBJECTS
OBJECTS = $(SRC:src/%.cpp=$(BUILDDIR)%.o)
ENGINEOBJS = $(ENGINESRC:src/engine/%.cpp=$(BUILDDIR)%.o)
ENGINEOUT = build/libengine.a

# define misc
CC = g++
RAYLIB ?= ./external/raylib-5.5

# flags
ENGINEFLAGS = $(ENGINEOUT) -L$(BUILDDIR) -lengine
RAYLIBFLAGS = -I $(RAYLIB) -L$(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LDFLAGS =

# target
OUT = engine
BUILDDIR = build/

game: $(ENGINEOUT) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BUILDDIR)$(OUT) $(LDFLAGS) $(RAYLIBFLAGS) $(ENGINEFLAGS)

engine: $(ENGINEOUT)

$(ENGINEOUT): $(ENGINEOBJS) # -c makes it not link
	ar rcs $(ENGINEOUT) $(ENGINEOBJS)

build/%.o: src/%.cpp
	$(CC) -c $< -o $@


$(BUILDDIR)%.o: src/engine/%.cpp
	$(CC) -c $< -o $@

clean:
	rm -r $(BUILDDIR)*

run: $(ENGINEOUT) $(OBJECTS) game
	$(BUILDDIR)$(OUT)

all: clean game
