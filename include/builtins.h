#ifndef BUILTINS_H_
#define BUILTINS_H_

#include<lexer.h>

/*
    cd
    exit
    export
    unset
    echo
    pwd
    env
    
    extra features:
    history
    alias/unalias
    source
    jobs, fg, bg
*/
typedef struct Builtin;

int isBuiltIn(Token*);
int runBuiltIn(char** , int);



#endif // BUILTINS_H_