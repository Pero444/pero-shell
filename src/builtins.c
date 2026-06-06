#include <stdio.h>
#include <unistd.h>

// int _exit() {
//     printf("Exiting.....");
//     return -1;
// }

void exitShell() {
    printf("Exiting shell...");
    
}

void changeDirectory(char* dir){
    chdir(dir);
}

