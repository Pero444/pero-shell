CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude 
LDFLAGS = -lreadline

SRC    := $(wildcard src/*.c)
OBJ    := $(patsubst src/%.c,build/%.o,$(SRC))

LIB_OBJ := $(filter-out build/main.o,$(OBJ))

TARGET := bin/pero
LEXER_TEST_TARGET := bin/lexer_test
PARSER_TEST_TARGET := bin/parser_test
EXECUTOR_TEST_TARGET := bin/executor_test

all: $(TARGET)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

lexer_test: $(LEXER_TEST_TARGET)
	./$(LEXER_TEST_TARGET)

parser_test: $(PARSER_TEST_TARGET)
	./$(PARSER_TEST_TARGET)

executor_test: $(EXECUTOR_TEST_TARGET)
	./$(EXECUTOR_TEST_TARGET)

$(TARGET): $(OBJ)
	mkdir -p bin
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(LEXER_TEST_TARGET): tests/lexerTest.c $(LIB_OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -Itests $< $(LIB_OBJ) -o $@ $(LDFLAGS)

$(PARSER_TEST_TARGET): tests/parserTest.c $(LIB_OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -Itests -Iinclude $< $(LIB_OBJ) -o $@ $(LDFLAGS)

$(EXECUTOR_TEST_TARGET): tests/executorTest.c $(LIB_OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) -Itests -Iinclude $< $(LIB_OBJ) -o $@ $(LDFLAGS)

clean:
	rm -rf build bin

.PHONY: all clean parser_test lexer_test executor_test