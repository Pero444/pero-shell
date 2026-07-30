#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "input.h"

const char* promptTexts[] = {
    "\001\033[0;31m\002prompt:~ # \001\033[0m\002$ ",
    " > "
};

char* takeInput(Prompt prompt) {

    char* line = readline(promptTexts[prompt]);
    
    // ctrl + d aka eof + empty string = exit returns NULL
    if(line == NULL) return NULL;

    // if not empty add to history
    if (line[0] != '\0') add_history(line);

    return line;
}

