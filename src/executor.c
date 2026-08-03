#include "executor.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"

/**
 * @brief
 * @param
 */
static void applyRedirection(SimpleCommand* cmd) {
    Redirect* r = cmd->redirects;

    while (r != NULL) {
        int fd;
        int targetFd;

        switch (r->type) {
            case REDIR_IN:
                fd = open(r->filename, O_RDONLY);
                targetFd = STDIN_FILENO;
                break;
            case REDIR_OUT:
                fd = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                targetFd = STDOUT_FILENO;
                break;
            case REDIR_APPEND:
                fd = open(r->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
                targetFd = STDOUT_FILENO;
                break;
            default:
                continue;
        }

        if (fd < 0) {
            fprintf(stderr, "pero: %s: %s\n", r->filename, strerror(errno));
            _exit(1); /* safe: we're always inside a forked child by the
                        time this runs */
        }

        r = r->next;

        dup2(fd, targetFd);
        close(fd);
    }
}

/**
 * @brief
 * @param
 * @return
 */
static int executePipeline(Executor* exec) {
    int n = exec->count;
    SimpleCommand* cmds = exec->pipeline->commands;

    // single command
    int builtinIdx = isBuiltIn(cmds[0].argv[0]);
    if (n == 1 && builtinIdx != -1) {
        SimpleCommand* cmd = &cmds[0];

        int saved_stdout = -1, saved_stdin = -1;
        if (cmd->redirects) {
            saved_stdout = dup(STDOUT_FILENO);
            saved_stdin = dup(STDOUT_FILENO);
            applyRedirection(cmd);
        }

        runBuiltIn(&cmd->argv[0], builtinIdx);

        if (cmd->redirects) {
            dup2(saved_stdout, STDOUT_FILENO);
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdout);
            close(saved_stdin);
        }
        return 0;
    }

    int pipes[n - 1 > 0 ? n - 1 : 1][2];
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return 1;
        }
    }

    pid_t pids[n];

    for (int i = 0; i < n; i++) {
        fflush(NULL);

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            return 1;
        }

        if (pid == 0) {
            // Child process
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < n - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < n - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // apply redirections
            applyRedirection(&cmds[i]);

            builtinIdx = isBuiltIn(cmds[i].argv[0]);
            if (builtinIdx != -1) {
                runBuiltIn(&cmds[i].argv[0], builtinIdx);
                fflush(stdout);
                _exit(0);
            }

            execvp(cmds[i].argv[0], cmds[i].argv);

            /* execvp only returns on failure */
            fprintf(stderr, "pero: %s: %s\n", cmds[i].argv[0],
                    "command not found...");

            _exit(127);
        }
        pids[i] = pid;
    }

    // Parent process

    for (int j = 0; j < n - 1; j++) {
        close(pipes[j][0]);
        close(pipes[j][1]);
    }

    if (exec->pipeline->background) {
        printf("[bg pid %d]\n", pids[n - 1]);
        return 0;
        // !!
    }

    int status = 0;
    for (int i = 0; i < n; i++) {
        waitpid(pids[i], &status, 0);
    }

    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

/**
 * @brief executes the pipeline
 * @param
 * @return
 */
int execute(Pipeline* p) {
    Executor exec = {p, 0, 0};
    exec.count = p->count;

    return executePipeline(&exec);
}