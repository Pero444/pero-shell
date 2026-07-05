#ifndef LEXER_H_
#define LEXER_H_

typedef enum {
    NONE,
    TOK_WORD,
    TOK_PIPE,
    TOK_REDIR_IN,
    TOK_REDIR_OUT,
    TOK_REDIR_APPEND,
    TOK_BACKGROUND,

} TokenType;

typedef struct {
    TokenType type;
    char* value;

} Token;

TokenType isType(char*);
Token* lexer(char*);
void printTokens(Token*);

#endif  // LEXER_H_