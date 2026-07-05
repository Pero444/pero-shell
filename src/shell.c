#include "shell.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// GNU readline
#include <readline/history.h>
#include <readline/readline.h>
//

#include "builtins.h"
#include "input.h"
#include "lexer.h"

int setConfig(config _Config) {
    _Config->homepath = getenv("HOME");
    _Config->username = getenv("USER");
    _Config->hostname = getenv("HOSTNAME");

    chdir(_Config->homepath);
    return 0;
}

void printCWD() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s", cwd);
}

// promjeni ovaj dio
char* prompt(config _Config) {
    char highlightColor[] = "\001\033[0;31m\002";
    char defaultColor[] = "\001\033[0m\002";
    char start[] = " (•`_´•) ";
    char userHostSep[] = ":";

    char* homepath = getenv("HOME");
    char* username = getenv("USER");
    char* hostname = getenv("HOSTNAME");

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    int size = strlen(highlightColor) + strlen(defaultColor) + strlen(start) +
               strlen(userHostSep) + strlen(homepath) + strlen(username) +
               strlen(hostname) + strlen(cwd) + 1;

    char* prompt = (char*)malloc(size * sizeof(char));
    snprintf(prompt, size, "%s %s@%s%s%s%s %s", highlightColor, username,
             hostname, userHostSep, cwd, start, defaultColor);

    return prompt;
}

void printTitle() {
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    printf("\n~~~~~                pero-shell               ~~~~~");
    printf("\n~~~~~             Use with caution            ~~~~~");
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void peroLoop(config _Config) {
    char* line;

    while (1) {
        line = takeInput();

        if (line == NULL) {
            printf("exit\n");
            break;
        }

        if (line[0] == '\0') {
            free(line);
            continue;
        }


        // // lexer - input line ---> tokens
        Token* tokens = lexer(line);

        // ast parser  tokens --->  ast node structure

        // executor 
        
        int idx = isBuiltIn(&tokens[0]);
        char** names;
        runBuiltIn(names, idx);
        // if (idx >= 0) {
        //     runBuiltIn(tokens[0].value, idx);
        // } else {
        //     printf("\nNot a built in command, trying to search...");
        // }
        
        


        // free line, tokens, ast nodes
        free(line);
        line = NULL;

        free(tokens);
        tokens = NULL;

        
        //free();
        //ast = NULL
    }
}
