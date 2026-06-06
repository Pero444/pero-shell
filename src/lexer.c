#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int countTokens(char* line) {
    int count = 0;
    bool inWord = false;

    for (int i = 0; line[i] != '\0'; i++) {
        // isspace() provjerava ' ' \t \n \r \f \v
        if (!isspace((unsigned char)line[i])) {
            if (!inWord) {
                count++;
                inWord = true;
            }
        } else {
            inWord = false;
        }
    }

    return count;
}

char** tokenize(char* line) {
    char* temp = strdup(line);
    int count = countTokens(temp);

    char** tokens = (char**)malloc((count + 1) * sizeof(char*));
    if (tokens == NULL) {
        printf("\nError allocating memory for tokens.");
        free(temp);
        return NULL;
    }

    int i = 0;
    char* token = strtok(temp, " \t");

    while (token != NULL) {
        tokens[i++] = strdup(token);
        token = strtok(NULL, " \t");
    }

    tokens[i] = NULL;

    // free memory
    free(temp);

    return tokens;
}

void freeTokens(char** tokens) {
    if (tokens == NULL) return;

    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}