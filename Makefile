# SRC
SRC = $(wildcard src/*.cpp) 
HSSRC = src/Raylib.hs src/Player.hs
ENGINESRC := $(wildcard src/engine/*.cpp)

# OBJECTS
OBJECTS = $(SRC:src/%.cpp=$(BUILDDIR)%.o) 
HSOBJECTS = $(HSSRC:src/%.hs=$(BUILDDIR)hs_%.o) 

ENGINEOBJS = $(ENGINESRC:src/engine/%.cpp=$(BUILDDIR)%.o)
ENGINEOUT = $(BUILDDIR)libengine.a

# Define Compilers
CC = g++
HC = ghc

# Flags
ENGINEFLAGS = $(ENGINEOUT) -L$(BUILDDIR) -lengine
LDFLAGS = -lraylib -lstdc++ -no-hs-main

# Target
OUT = build/engine
BUILDDIR = build/

$(OUT): $(ENGINEOUT) $(OBJECTS) $(HSOBJECTS) $(BUILDDIR)
	$(HC) $(ENGINEFLAGS) $(OBJECTS) $(HSOBJECTS) -o $(OUT) $(LDFLAGS) 

$(ENGINEOUT): $(ENGINEOBJS)
	ar rcs $(ENGINEOUT) $(ENGINEOBJS)

build/%.o : src/engine/%.cpp
	$(CC) -c $< -o $@

build/%.o : src/%.cpp
	$(CC) -c $< -o $@

$(BUILDDIR)hs_%.o : src/%.hs
	$(HC) -c $< -o $@ -outputdir $(BUILDDIR) -Isrc

$(BUILDDIR):
	mkdir $(BUILDDIR)

clean:
	rm -r $(BUILDDIR)*

run: $(OUT)
	$(OUT)
