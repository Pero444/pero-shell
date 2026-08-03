#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"

typedef struct {
    char* name;
    int (*func)(char** argv);

} Builtin;

int builtin_cd(char** argv) {
    // if there are no arguments move to HOME path
    if (argv[1] == NULL) {
        chdir(getenv("HOME"));
        return 0;
    }

    chdir(argv[1]);
    return 0;
}

/**
 *
 */
int builtin_exit(char** argv) {
    int code = EXIT_SUCCESS;

    if (argv[1] != NULL) {
        code = atoi(argv[1]);
    }

    printf("exit\n");
    exit(code);

    return 0;
}
int builtin_export(char** argv) {
    printf("\nexport");
    for (int i = 1; argv[i]; i++) {
        fputs(argv[i], stdout);
        if (argv[i + 1]) fputc(' ', stdout);
    }
    fputc('\n', stdout);
    return 0;
}

int builtin_unset(char** argv) {
    printf("\nunset");
    for (int i = 1; argv[i]; i++) {
        fputs(argv[i], stdout);
        if (argv[i + 1]) fputc(' ', stdout);
    }
    fputc('\n', stdout);
    return 0;
}

int builtin_echo(char** argv) {
    for (int i = 1; argv[i]; i++) {
        fputs(argv[i], stdout);
        if (argv[i + 1]) fputc(' ', stdout);
    }
    fputc('\n', stdout);
    return 0;
}

int builtin_env(char** argv) {
    printf("\nenv");
    for (int i = 1; argv[i]; i++) {
        fputs(argv[i], stdout);
        if (argv[i + 1]) fputc(' ', stdout);
    }
    fputc('\n', stdout);
    return 0;
}

int builtin_pwd(char** argv) {
    (void)argv; /* pwd takes no meaningful arguments in your scope */

    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("pwd");
        return 1;
    }

    printf("%s\n", cwd);
    return 0;
}

static Builtin builtins[] = {
    {"cd", builtin_cd},         {"exit", builtin_exit},
    {"export", builtin_export}, {"unset", builtin_unset},
    {"echo", builtin_echo},     {"env", builtin_env},
    {"pwd", builtin_pwd},       {NULL, NULL},
};

//
int isBuiltIn(char* cmd) {
    // for every i builtin until NULL, check if exist
    for (int i = 0; builtins[i].name; i++) {
        if (strcmp(cmd, builtins[i].name) == 0) return i;
    }
    return -1;  // if not builtin
}

int runBuiltIn(char** argv, int idx) {
    builtins[idx].func(argv);
    return 0;
}