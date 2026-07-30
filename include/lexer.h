#ifndef LEXER_H_
#define LEXER_H_

typedef enum {
    NONE,
    TOK_WORD,           // grep
    TOK_PIPE,           // |
    TOK_REDIR_IN,       // <
    TOK_REDIR_OUT,      // >
    TOK_REDIR_APPEND,   // >>
    TOK_BACKGROUND,     // &
    TOK_LPAREN,         // (
    TOK_RPAREN,         // )
    TOK_EOF

} TokenType;

typedef struct {
    TokenType type;
    char* value;

} Token;

TokenType isType(char*);
Token* lexer(char*);
void printTokens(Token*);
void freeTokens(Token*);

#endif  // LEXER_H_