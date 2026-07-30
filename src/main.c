#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"

int main(int argc, char* argv[]) {

    //load configs??
    Config CONFIG;
    if(setConfig(&CONFIG)) {
        printf("\nconfig not setup\n");
    }

    printTitle();
    
    //loop time
    peroLoop(&CONFIG);
    
    return 0;
}