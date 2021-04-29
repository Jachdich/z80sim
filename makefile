SOURCES := $(shell find src -type f -name *.c)
HEADERS := $(shell find include -type f -name *.h)
OBJECTS := $(patsubst src/%,obj/%,$(SOURCES:.c=.o))

z80sim: $(OBJECTS)
	gcc $(OBJECTS) -o $@

obj/%.o: src/%.c $(HEADERS)
	@mkdir -p obj
	gcc -c -o $@ $< -Wall -Werror -g -ggdb -Iinclude

clean:
	rm obj/*.o
	rm z80sim

.PHONY: clean

