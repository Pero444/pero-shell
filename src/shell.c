#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell.h"
#include "input.h"
#include "lexer.h"
#include "parser.h"
#include "executor.h"

/**
 * @brief prints the title and warning
 */
void printTitle() {
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("~~~~~                pero-shell               ~~~~~\n");
    printf("~~~~~             Use with caution            ~~~~~\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

/**
 * @brief main loop that runs the whole shell, REPL loop that does everything u
 * sequence input reading, tokenzation, tree parsing, and execution, memory
 * allocation and dealocation.
 * @param _Config that will hold said values
 */
int peroLoop() {
    char* line;

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
        //printTokens(tokens);
        
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

        // printPipeline(pipeline);

        // executor - pipeline ---> command execution
        execute(pipeline);
        freePipeline(pipeline);
        pipeline = NULL;

        
        // free line
        free(line);
        line = NULL;

    }

    shutdown:
    return 0;
}
