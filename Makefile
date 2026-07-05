CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude 
LDFLAGS = -lreadline

SRC    := $(wildcard src/*.c)
OBJ    := $(patsubst src/%.c,build/%.o,$(SRC))

TARGET := bin/pero

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build bin

.PHONY: all clean