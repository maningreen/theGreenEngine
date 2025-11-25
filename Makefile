# SRC
SRC = $(wildcard src/*.cpp) 
HSSRC = $(wildcard src/*.hs)
ENGINESRC = $(wildcard src/engine/*.cpp)

# OBJECTS
OBJECTS = $(SRC:src/%.cpp=$(BUILDDIR)%.o) 
HSOBJECTS = $(HSSRC:src/%.hs=$(BUILDDIR)hs_%.o) 

ENGINEOBJS = $(ENGINESRC:src/engine/%.cpp=$(BUILDDIR)%.o)
ENGINEOUT = $(BUILDDIR)libengine.a

# Define Compilers
CC = g++
HC = ghc

RAYLIB = ./external

# Flags
ENGINEFLAGS = $(ENGINEOUT) -L$(BUILDDIR) -lengine
LDFLAGS = -lraylib -lstdc++ -no-hs-main -i./src -i./.
RAYLIBFLAGS = -I$(RAYLIB) -L$(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Target
OUT = build/engine
BUILDDIR = build/

HSDEPS = $(BUILDDIR)hsDeps.mk

$(OUT): $(ENGINEOUT) $(OBJECTS) $(HSOBJECTS) $(BUILDDIR)
	$(HC) $(ENGINEFLAGS) $(OBJECTS) $(HSOBJECTS) -o $(OUT) $(LDFLAGS) $(RAYLIBFLAGS)

$(ENGINEOUT): $(ENGINEOBJS)
	ar rcs $(ENGINEOUT) $(ENGINEOBJS)

build/%.o : src/engine/%.cpp
	$(CC) -c $< -o $@ -I$(RAYLIB)

build/%.o : src/%.cpp
	$(CC) -c $< -o $@ -I$(RAYLIB)

$(BUILDDIR)hs_%.o : src/%.hs $(HSDEPS) | $(BUILDDIR)
	$(HC) -c $< -hidir $(BUILDDIR) -outputdir $(BUILDDIR) -o $@

$(HSDEPS): $(HSSRC) | $(BUILDDIR)
	$(HC) -isrc -M $(HSSRC) -odir $(BUILDDIR) -hidir $(BUILDDIR) -dep-makefile $(HSDEPS)
	sed -e 's!$(BUILDDIR)!$(BUILDDIR)hs_!g'\
			-e 's!\.hi!\.o!g' $(HSDEPS) > $(HSDEPS).tmp
	mv $(HSDEPS).tmp $(HSDEPS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -r $(BUILDDIR)

run: $(OUT)
	$(OUT)

include $(HSDEPS)

.PHONY: clean run
