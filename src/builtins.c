#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* name;
    int (*func)(char** argv);

} Builtin;

int builtin_cd(char** argv) { 
    chdir(argv[1]); 
}
int builtin_exit(char** argv) { 
    printf("Closing shell...\n");
    exit(EXIT_SUCCESS);
}
int builtin_export(char** argv) { printf("\nexport"); }
int builtin_unset(char** argv) { printf("\nunset"); }
int builtin_echo(char** argv) { 
    printf("\n%s", argv[1]);   
 }
int builtin_env(char** argv) { printf("\nenv"); }

static Builtin builtins[] = {
    {"cd", builtin_cd},
    {"exit", builtin_exit},
    {"export", builtin_export},
    {"unset", builtin_unset},
    {"echo", builtin_echo},
    {"env", builtin_env},
    {NULL, NULL},
};




int isBuiltIn(char* cmd) {
    for (int i = 0; builtins[i].name; i++){
        if(strcmp(cmd, builtins[i].name) == 0) return i;
    }
    return -1; // if not builtin
}

int runBuiltIn(char** argv, int i) {
    builtins[i].func(argv);
}