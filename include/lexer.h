#ifndef LEXER_H_
#define LEXER_H_

int countTokens(char*);
char** tokenize(char*);

void freeTokens(char**);

#endif // LEXER_H_