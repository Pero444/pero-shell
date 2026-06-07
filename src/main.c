#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "input.h"
#include "executor.h"
#include "lexer.h"
#include "builtins.h"

int main(int argc, char* argv[]) {

    //load configs??
    Config CONFIG;
    if(setConfig(&CONFIG)) {
        printf("\nconfig not setup\n");
    }

    printTitle();
    
    //loop time
    peroLoop(&CONFIG);
    
    
    // cleanup
    return 0;
}