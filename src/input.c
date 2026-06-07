#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>

/*
char* takeInput() {
    char* line = NULL;

    size_t len = 0;
    if (getline(&line, &len, stdin) == -1) {
        if (feof(stdin)) {
            free(line);
            exit(0);
        } else {
            perror("getline");
            free(line);
            exit(1);
        }
    }

    if (line[0] == '\0' || line[0] == '\n') return NULL;
    // line[strlen(line) - 1] = '\0';
    line[strcspn(line, "\n")] = '\0';

    return line;
}
*/

char* takeInput() {
    char* line = readline("\001\033[0;31m\002prompt:~ # \001\033[0m\002$ ");
    
    // ctrl + d aka eof + empty string = exit returns NULL
    if(line == NULL) return NULL;

    // if not empty add to history
    if (line[0] != '\0') add_history(line);

    return line;
}
