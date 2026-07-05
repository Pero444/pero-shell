#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

int count(char* line) {
    int count = 0;
    bool inWord = false;

    for (int i = 0; line[i] != '\0'; i++) {
        // isspace() provjerava ' ' \t \n \r \f \v
        if (!isspace((unsigned char)line[i])) {
            if (!inWord) {
                count++;
                inWord = true;
            }
        } else {
            inWord = false;
        }
    }

    return count;
}

Token* lexer(char* line) {
    char* temp = strdup(line);
    int tokenCount = count(temp);

    Token* tokens = (Token*)malloc((tokenCount + 1) * sizeof(Token));
    
    // char** tokens = (char**)malloc((tokenCount + 1) * sizeof(char*));
    if (tokens == NULL) {
        printf("\nError allocating memory for tokens.");
        free(temp);
        return NULL;
    }
    

    int i = 0;
    char* token = strtok(temp, " \t");

    while (token != NULL) {
        tokens[i].value = strdup(token);
        tokens[i].type = isType(token);
        i++;
        token = strtok(NULL, " \t");
    }

    tokens[i].value = NULL;
    tokens[i].type = NONE;

    // free memory
    free(temp);

    return tokens;
}

TokenType isType(char* token) {
    if (strcmp(token, "|") == 0) {
        return TOK_PIPE;
    } else if (strcmp(token, ">") == 0) {
        return TOK_REDIR_OUT;
    } else if (strcmp(token, "<") == 0) {
        return TOK_REDIR_IN;
    } else if (strcmp(token, ">>") == 0) {
        return TOK_REDIR_APPEND;
    } else if (strcmp(token, "&") == 0) {
        return TOK_BACKGROUND;
    } else return TOK_WORD;
}

void printTokens(Token* tokens) {
    int i = 0;
    while(tokens[i].value != NULL) {
        printf("\n%s - %d", tokens[i].value, tokens[i].type);
        i++;
    }
    printf("\n");
}
