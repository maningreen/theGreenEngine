# SRC
SRC = $(wildcard src/*.cpp)
ENGINESRC = $(wildcard src/engine/*.cpp)

# OBJECTS
OBJECTS = $(SRC:src/%.cpp=$(BUILDDIR)%.o)
ENGINEOBJS = $(ENGINESRC:src/engine/%.cpp=$(BUILDDIR)%.o)
ENGINEOUT = build/libengine.a

# define misc
CC = g++

# Flags
ENGINEFLAGS = $(ENGINEOUT) -L$(BUILDDIR) -lengine
LDFLAGS = -lraylib

# Target
OUT = build/engine
BUILDDIR = build/

$(OUT): $(ENGINEOUT) $(OBJECTS) $(BUILDDIR)
	$(CC) $(OBJECTS) -o $(OUT) $(LDFLAGS) $(ENGINEFLAGS)

$(ENGINEOUT): $(ENGINEOBJS)
	ar rcs $(ENGINEOUT) $(ENGINEOBJS)

build/%.o: src/engine/%.cpp
	$(CC) -c $< -o $@

build/%.o: src/%.cpp
	$(CC) -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -r $(BUILDDIR)*

run: $(ENGINEOUT) $(OBJECTS) $(OUT)
	$(OUT)
