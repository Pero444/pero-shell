#include <stdio.h>
#include <unistd.h>
//#include <shell.h>
//#include <input.h>

void printPrompt() {
    printf("\n<-/");
}

void printTitle() {
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    printf("\n~~~~~                pero-shell               ~~~~~");
    printf("\n~~~~~~~~~~~~~     Use with caution    ~~~~~~~~~~~~~");
    printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
}

int main(int argc, char* argv[]) {

    printTitle();
    printPrompt();

    char cwd[1024];
    
    while(1) {
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    }

    }
    
    return 0;
}