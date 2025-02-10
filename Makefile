SRC = src/*.cpp
BUILDDIR = build/
OUT = engine
LDFLAGS = -lraylib
CC = g++

program:
	$(CC) $(SRC) $(LDFLAGS) -o $(BUILDDIR)$(OUT)
