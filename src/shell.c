#include "shell.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void printPrompt(config _Config) {
    printf("\033[0;31m");
    printf("\n%s@%s", _Config->username, _Config->hostname);
    printf("\033[0m");
    printf(":");
    printf("\033[0;31m");
    printCWD();
    printf(" <-_-> ");
    printf("\033[0m");
}

void printTitle() {
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    printf("\n~~~~~                pero-shell               ~~~~~");
    printf("\n~~~~~             Use with caution            ~~~~~");
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void peroLoop(config _Config) {
    char* line;
    // int lineSize = sizeof(line);

    while (1) {
        printPrompt(_Config);

        line = takeInput();
        if (line == NULL) {
            continue;
        }
        

        // lexer - tokenizacija
        char** tokens = tokenize(line);

        int idx = isBuiltIn(tokens[0]);
        if (idx >= 0) {
            runBuiltIn(tokens, idx);
        } else {
            printf("\nNot a built in command, trying to search...");
        }



        // executor



        // free all
        // free tokens
        freeTokens(tokens);
        tokens = NULL;
        
                
    }
    freeInput(line);
    line = NULL;

}
