builddir = build
srcdir = src

src := $(shell find $(srcdir) -name "*.cpp") $(shell find $(srcdir) -name "*.hs")

HC = ghc
CXX = g++
CXXFLAGS = -lstdc++ -Iexternal -Lexternal -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -llua
TARGET = $(builddir)/out

OBJ := $(src:$(srcdir)/%.cpp=$(builddir)/%.o)

build: $(TARGET)

$(TARGET): $(OBJ)
	$(HC) $^ -o $@ $(CXXFLAGS) -lstdc++ -no-hs-main -hidir $(builddir) -outputdir $(builddir) 

build/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(builddir)

run: $(TARGET)
	$(TARGET)

.PHONY: build clean run
