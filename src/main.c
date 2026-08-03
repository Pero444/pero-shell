#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell.h"

int main(int argc, char* argv[]) {
    (void)argv;

    // Currently the shell does not have any input features
    //  like pero script.sh arg1 arg2
    if (argc > 1) {
        printf(
            "pero-shell currently doesn't have any scripting or input features, "
            "it's as simple as they come.\n");
        return 0;
    }

    printTitle();

    peroLoop();

    return 0;
}