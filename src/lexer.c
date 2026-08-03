#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

/**
 * @brief Determines the number of words in a input string.
 * @param line input string that holds all the tokens
 * @return Returns the number of words in string.
 */
int count(char* line) {
    int count = 0;
    bool inWord = false;

    for (int i = 0; line[i] != '\0'; i++) {
        // isspace() checks ' ' \t \n \r \f \v
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

/**
 * @brief Divides the input string into smaller chunks called tokens.
 * @param line input strings that holds all the tokens
 * @return Returns pointer to an array of tokens the function creates.
 */
Token* lexer(char* line) {
    char* temp = strdup(line);
    int tokenCount = count(temp);

    Token* tokens = malloc((tokenCount + 1) * sizeof(Token));

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
    tokens[i].type = TOK_EOF;

    free(temp);

    return tokens;
}


/**
 * @brief Frees up the memory allocated for tokens.
 * @param tokens tokens to be freed
 */
void freeTokens(Token* tokens) {
    int i = 0;
    while(tokens[i].type != TOK_EOF) {
        free(tokens[i].value);
        i++;
    }
    free(tokens[i].value);
    free(tokens);
}

/**
 * @brief Determines the type of token.
 * @param token pointer to a token to check
 * @return Returns enum tokenType.
 */
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

    } else if (strcmp(token, "(") == 0) {
        return TOK_LPAREN;

    } else if (strcmp(token, ")") == 0) {
        return TOK_RPAREN;

    } else return TOK_WORD;
}

/**
 * @brief Lookup table for token types
 * for debugging purposes.
 * @param type token type 
 * @return Returns string matching token type.
 */
const char* getTypeString(TokenType type) {
    static const char* types[] = {
        "NONE",           
        "TOK_WORD",      
        "TOK_PIPE",            
        "TOK_REDIR_IN",   
        "TOK_REDIR_OUT", 
        "TOK_REDIR_APPEND",
        "TOK_BACKGROUND", 
        "TOK_LPAREN",         
        "TOK_RPAREN", 
        "TOK_EOF"
    };

    if (type < 0 || type >= sizeof(types) / sizeof(types[0])) {
        return "UNKNOWN";
    }

    return types[type];
}

/**
 * @brief Token that prints all the tokens.
 * @param tokens tokens to be printed to terminal
 */
void printTokens(Token* tokens) {
    int i = 0;
    
    printf("\n--------------TOKENS------------\n");
    while (tokens[i].value != NULL) {  
        printf("\n\e[1m%d.\e[m - %s\t - %s", i, tokens[i].value, getTypeString(tokens[i].type));
        i++;
    }
    printf("\n---------------------------------\n");  
}