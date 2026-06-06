#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
char* takeInput(int size) {
    char* line = (char*)malloc(size * sizeof(char));

    if (fgets(line, size, stdin) == NULL) {
        printf("\nUnable to read input.");
    }

    if (strlen(line) <= 0) return NULL;

    line[strlen(line) - 1] = '\0';
    return line;
}
*/

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

void freeInput(char* line) { free(line); }