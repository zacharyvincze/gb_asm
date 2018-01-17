CXX ?= g++
MKDIR := mkdir -p
CXXFLAGS := -pedantic -std=c++14
PROG := bin/gb_asm
OBJS := $(patsubst src/%.cpp,obj/%.o, $(wildcard src/*.cpp))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: build

build: $(PROG)

-include $(DEPS)

clean:
	rm -rf $(PROG) $(OBJS)

$(PROG): $(OBJS)
	@$(MKDIR) $(dir $@)
	$(CXX) $^ $(CXXFLAGS) -o $@

obj/%.o : src/%.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) $< $(CXXFLAGS) -c -MD -o $@