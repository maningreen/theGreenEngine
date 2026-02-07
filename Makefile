builddir = build
srcdir = src

src := $(shell find $(srcdir) -name "*.cpp") $(shell find $(srcdir) -name "*.hs")

HC = ghc
HCFLAGS = -lstdc++ -no-hs-main -hidir $(builddir) -outputdir $(builddir) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -llua -Iexternal -Lexternal
CXX = g++
CXXFLAGS = -Iexternal -Lexternal -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -llua
TARGET = $(builddir)/out

OBJ := $(src:$(srcdir)/%.cpp=$(builddir)/%.o)

build: $(TARGET)

$(TARGET): $(OBJ)
	$(HC) $^ -o $@ $(HCFLAGS) -lstdc++ -no-hs-main -hidir $(builddir) -outputdir $(builddir) 

build/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(builddir)

run: $(TARGET)
	$(TARGET)

.PHONY: build clean run
