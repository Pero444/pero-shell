#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

// AST

typedef enum {
    REDIR_IN,
    REDIR_OUT,
    REDIR_APPEND

} RedirType;

typedef enum {
    PARSE_OK,
    PARSE_ERROR,
    PARSE_INCOMPLETE

} parse_status;

typedef enum {
    CMD_SIMPLE,
    CMD_SUBSHELL

} CommandType;

typedef struct _Redirect {
    RedirType type;
    char* filename;
    struct _Redirect* next;

} Redirect;

typedef struct {
    char** argv;
    int argc;
    int argv_capacity;
    Redirect* redirects;

} SimpleCommand;


typedef struct {
    SimpleCommand* commands;
    int count;
    int capacity;
    int background;
    parse_status status;

} Pipeline;


/*

Maybe in future add nested pipes and recursions

typedef struct {
    CommandType type;

    union {
        SimpleCommand simple;
        Pipeline* subshell;
    };

    Redirect* redirects;
} Command;
*/

/**
 * @brief parser structure that holds all the tokens
 * and the index of current token worked on
 * @brief The parser is the root of the parsing tree
 */
typedef struct {
    Token* tokens;
    int pos;
    parse_status status;

} Parser;


Pipeline* parse(Token* tokens);

void printPipeline(Pipeline* p);

void freePipeline(Pipeline* p);

#endif  // PARSER_H_