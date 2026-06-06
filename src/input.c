#include <stdio.h>
#include <string.h>
#include <ctype.h>

int readInput(char* line, int size) {

    //reset line to empty string
    memset(line, 0, size);

    if(fgets(line, size, stdin) == NULL) {
        printf("No input.\n");
        return 1;
    }

    // remove end of line char
    line[strlen(line) - 1] = '\0';
    return 0;
}

void formatInput(char* line) {

    char* temp;
    strcpy(temp, line);
    printf("%s\n", temp);
    while(isspace(temp[0])){
        ++temp;
    }

    printf("%s\n", temp);

    int isPrevSpace = 0;
    for(int i = 0; i < strlen(temp); i++) {
        if(isspace(temp[i])) {
            isPrevSpace = 1;
        }
    }
}