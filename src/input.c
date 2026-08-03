#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "input.h"

/**
 * @brief Prompt table to be used. The prompt is fetched using prompt type enum.
 */
const char* promptTexts[] = {
    "đe je pero",
    " > "
};

/**
 * @brief Function that builds the prompt.
 * @return Returns the prompt.
 */
char* getPrompt() {
    char highlightColor[] = "\001\033[0;31m\002";
    char defaultColor[] = "\001\033[0m\002";
    char start[] = "#";

    char* homepath = getenv("HOME");
    char* username = getenv("USER");
    char* hostname = getenv("HOSTNAME");

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    char* recwd;
    char* sep;
    int homelen = strlen(homepath);

    if(strncmp(cwd, homepath, homelen) == 0 &&
    (cwd[homelen] == '/' || cwd[homelen] == '\0')) {
        recwd = cwd + strlen(homepath);
        sep = ":~";
    }
    else {
        recwd = cwd;
        sep = ":";
    }


    int size = strlen(highlightColor) + strlen(defaultColor) + strlen(start) +
               strlen(sep) + strlen(username) + strlen(hostname) + strlen(recwd) + 3;

    char* prompt = malloc(size * sizeof(char));
    snprintf(prompt, size, "%s%s@%s%s%s%s%s ", highlightColor, username,
             hostname, sep, recwd, start, defaultColor);

    return prompt;
}

/**
 * @brief Function that reads the input from user and prints the prompt to terminal.
 * @param prompt type of prompt to use
 * @return Returns user input.
 */
char* takeInput(Prompt prompt) {
    char* line = readline((prompt == DEFAULT_PROMPT)? getPrompt(): promptTexts[prompt]);
    
    // ctrl + d aka eof + empty string = exit returns NULL
    if(line == NULL) return NULL;

    // if not empty add to history
    if (line[0] != '\0') add_history(line);

    return line;
}

