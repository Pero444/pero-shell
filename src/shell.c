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
#include "parser.h"

/**
 * @brief sets configuration for shell, such as homepath, user & hostname
 * @param _Config that will hold said values
 * @return returns zero on success, othewise 1 as failure
 */
int setConfig(config _Config) {
    _Config->homepath = getenv("HOME");
    _Config->username = getenv("USER");
    _Config->hostname = getenv("HOSTNAME");

    chdir(_Config->homepath);
    return 0;
}

/**
 * @brief prints current working directory
 * utilizes system call getcwd
 */
void printCWD() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) printf("%s", cwd);
}

/**
 * @brief build the shell prompt
 * @param _Config that will hold said values
 * @return returns prompt for printing
 */
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

/**
 * @brief prints the title and warning
 */
void printTitle() {
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    printf("\n~~~~~                pero-shell               ~~~~~");
    printf("\n~~~~~             Use with caution            ~~~~~");
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

/**
 * @brief main loop that runs the whole shell, REPL loop that does everything u
 * sequence input reading, tokenzation, tree parsing, and execution, memory
 * allocation and dealocation
 * @param _Config that will hold said values
 */
int peroLoop(config _Config) {
    char* line;
    int exit = 0;

    while (1) {
        line = takeInput(DEFAULT_PROMPT);

        if (line == NULL) {
            printf("exit\n");
            break;
        }

        if (line[0] == '\0') {
            free(line);
            continue;
        }

        // lexer - input line ---> tokens
        Token* tokens = lexer(line);

        // printTokens(tokens);

        // parser - token ---> parsed tree
        Pipeline* pipeline = parse(tokens);

        freeTokens(tokens);
        tokens = NULL;

        // if last parse is incomplete finish it
        while (pipeline->status == PARSE_INCOMPLETE) {
            freePipeline(pipeline);
            char* more = takeInput(INCOMPLETE_PROMPT);

            if (more == NULL) {
                printf("exit\n");
                printf("Pero-shell: unexpected EOF.\n");
                free(line);
                free(more);
                goto shutdown;
                break;
            }

            if (more[0] == '\0') {
                free(more);
                continue;
            }

            char* combined = malloc(strlen(line) + strlen(more) + 2);
            sprintf(combined, "%s %s", line, more);

            free(line);
            free(more);
            line = combined;
            tokens = lexer(line);
            pipeline = parse(tokens);
            freeTokens(tokens);
        }

        if (pipeline->status == PARSE_ERROR) {
            printf("Pero-shell: syntax error near unexpected token '|'\n");
            freePipeline(pipeline);
            free(line);
            continue;
        }

        // exectute pipeline

        printPipeline(pipeline);

        // executor - pipeline ---> command execution

        // free line, tokens, pipeline
        free(line);
        line = NULL;

        freePipeline(pipeline);
        pipeline = NULL;
    }

    shutdown:
    return 0;
}
