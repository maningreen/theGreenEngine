# SRC
SRC = $(wildcard src/*.cpp)
ENGINESRC = $(wildcard src/engine/*.cpp)

# OBJECTS
OBJECTS = $(SRC:src/%.cpp=$(BUILDDIR)%.o)
ENGINEOBJS = $(ENGINESRC:src/engine/%.cpp=$(BUILDDIR)%.o)
ENGINEOUT = build/libengine.a

# define misc
CC = g++

# flags
ENGINEFLAGS = $(ENGINEOUT) -L$(BUILDDIR) -lengine
LDFLAGS = -lraylib

# target
OUT = engine
BUILDDIR = build/

game: $(ENGINEOUT) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BUILDDIR)$(OUT) $(LDFLAGS) $(ENGINEFLAGS)

engine: $(ENGINEOUT)

$(ENGINEOUT): $(ENGINEOBJS) # -c makes it not link
	ar rcs $(ENGINEOUT) $(ENGINEOBJS)

$(BUILDDIR)%.o: src/%.cpp $(BUILDDIR)
	$(CC) -c $< -o $@

$(BUILDDIR)%.o: src/engine/%.cpp $(BUILDDIR)
	$(CC) -c $< -o $@

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -r $(BUILDDIR)*

run: $(ENGINEOUT) $(OBJECTS) game
	$(BUILDDIR)$(OUT)
