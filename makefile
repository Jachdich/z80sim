SOURCES := $(shell find src -type f -name *.cpp)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

z80sim: $(OBJECTS)
	g++ $(OBJECTS) -o $@

obj/%.o: src/%.cpp $(HEADERS)
	@mkdir -p obj
	g++ -c -o $@ $< -Wall -Werror -g -ggdb -Iinclude

clean:
	rm obj/*.o
	rm z80sim

.PHONY: clean

