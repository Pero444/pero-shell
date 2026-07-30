CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude 
LDFLAGS = -lreadline

SRC    := $(wildcard src/*.c)
OBJ    := $(patsubst src/%.c,build/%.o,$(SRC))

LIB_OBJ := $(filter-out build/main.o,$(OBJ))

TARGET := bin/pero
LEXER_TEST_TARGET := bin/lexer_test
PARSER_TEST_TARGET := bin/parser_test

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

lexer_test: $(LEXER_TEST_TARGET)
	./$(LEXER_TEST_TARGET)

$(LEXER_TEST_TARGET): tests/lexerTest.c $(LIB_OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -Itests $< $(LIB_OBJ) -o $@ $(LDFLAGS)

$(PARSER_TEST_TARGET): tests/parserTest.c $(LIB_OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -Itests -Iinclude $< $(LIB_OBJ) -o $@ $(LDFLAGS)

parser_test: $(PARSER_TEST_TARGET)
	./$(PARSER_TEST_TARGET)

clean:
	rm -rf build bin

.PHONY: all clean test