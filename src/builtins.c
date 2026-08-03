#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"

extern char** environ;

/**
 * @brief maps a builtin command name to its implementing function
 */
typedef struct {
    char* name;
    int (*func)(char** argv);

} Builtin;

/**
 * @brief changes the shell's current working directory. With no arguments,
 * moves to the user's HOME directory (matching real shell `cd` behavior).
 * @param argv argv[0] is "cd"; argv[1], if present, is the target path
 * @return always returns 0
 */
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
 * @brief terminates the shell process itself. Must run in the shell's own
 * process (never a forked child) since chdir()/exit() in a child would
 * have no effect on the parent shell.
 * @param argv argv[1], if present, is parsed as the exit code; defaults
 * to EXIT_SUCCESS if omitted
 * @return never returns — exit() terminates the process; the trailing
 * return 0 only exists to satisfy the compiler's -Wreturn-type check
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

/**
 * @brief 
 * @param 
 * @return 
 */
int builtin_export(char** argv) {
    (void)argv;
    return 0;
}

/**
 * @brief 
 * @param 
 * @return 
 */
int builtin_unset(char** argv) {
    (void)argv;
    return 0;
}

/**
 * @brief prints its arguments separated by single spaces, followed by a
 * trailing newline
 * @param argv arguments following "echo"
 * @return always returns 0
 */
int builtin_echo(char** argv) {
    for (int i = 1; argv[i]; i++) {
        fputs(argv[i], stdout);
        if (argv[i + 1]) fputc(' ', stdout);
    }
    fputc('\n', stdout);
    return 0;
}

/**
 * @brief prints every current environment variable, one per line, in
 * "KEY=VALUE" form.
 * @param argv unused
 * @return always returns 0
 */
int builtin_env(char** argv) {
    (void)argv;
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
    return 0;
}
/**
 * @brief prints current working directory
 * @param argv not used
 * @return Returns 0 on success otherwise fail
 */
int builtin_pwd(char** argv) {
    (void)argv;
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("pwd");
        return 1;
    }

    printf("%s\n", cwd);
    return 0;
}

/**
 * @brief lookup table of builtin commands
 */
static Builtin builtins[] = {
    {"cd", builtin_cd},         {"exit", builtin_exit},
    {"export", builtin_export}, {"unset", builtin_unset},
    {"echo", builtin_echo},     {"env", builtin_env},
    {"pwd", builtin_pwd},       {NULL, NULL},
};

/**
 * @brief Checks whether the input command is a builtin command.
 * @param cmd input command
 * @return Returns -1 if command is not builtin, else
 * returns the index of builtin command.
 */
int isBuiltIn(char* cmd) {
    for (int i = 0; builtins[i].name; i++) {
        if (strcmp(cmd, builtins[i].name) == 0) return i;
    }

    return -1;
}

/**
 * @brief Executes the builtin command
 * @param argv list of arguments for commands
 * @param idx index of builtin for lookup table
 * @return Returns -1 on failure, otherwise 0
 */
int runBuiltIn(char** argv, int idx) {
    if (!builtins[idx].func(argv)) {
        return 0;
    }

    return -1;
}