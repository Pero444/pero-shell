#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"

#include "executor.h"

static Token mk(TokenType type, const char* value) {
    Token t; t.type = type; t.value = value ? strdup(value) : NULL; return t;
}

static void run(const char* label, Token* tokens) {
    printf("\n=== %s ===\n", label);
    Pipeline* p = parse(tokens);
    if (p->status != PARSE_OK) {
        printf("(parse failed, status != PARSE_OK)\n");
        freePipeline(p);
        return;
    }
    int code = execute(p);
    printf("(exit code: %d)\n", code);
    freePipeline(p);
}

int main(void) {
    /* 1. single command */
    Token t1[] = { mk(TOK_WORD, "echo"), mk(TOK_WORD, "hello"), mk(TOK_EOF, NULL) };
    run("echo hello", t1);

    /* 2. pipeline: ls | wc -l */
    Token t2[] = {
        mk(TOK_WORD, "ls"), mk(TOK_PIPE, "|"),
        mk(TOK_WORD, "wc"), mk(TOK_WORD, "-l"), mk(TOK_EOF, NULL)
    };
    run("ls | wc -l", t2);

    /* 3. three-stage pipeline: echo -e "b\na\nc" | sort | head -1 */
    Token t3[] = {
        mk(TOK_WORD, "printf"), mk(TOK_WORD, "b\\na\\nc\\n"), mk(TOK_PIPE, "|"),
        mk(TOK_WORD, "sort"), mk(TOK_PIPE, "|"),
        mk(TOK_WORD, "head"), mk(TOK_WORD, "-1"), mk(TOK_EOF, NULL)
    };
    run("printf b/a/c | sort | head -1  (expect: a)", t3);

    /* 4. output redirection */
    Token t4[] = {
        mk(TOK_WORD, "echo"), mk(TOK_WORD, "written-to-file"),
        mk(TOK_REDIR_OUT, ">"), mk(TOK_WORD, "/tmp/pero_test_out.txt"),
        mk(TOK_EOF, NULL)
    };
    run("echo written-to-file > /tmp/pero_test_out.txt", t4);
    printf("file contents: ");
    fflush(stdout);
    system("cat /tmp/pero_test_out.txt");

    /* 5. append redirection */
    Token t5[] = {
        mk(TOK_WORD, "echo"), mk(TOK_WORD, "appended-line"),
        mk(TOK_REDIR_APPEND, ">>"), mk(TOK_WORD, "/tmp/pero_test_out.txt"),
        mk(TOK_EOF, NULL)
    };
    run("echo appended-line >> /tmp/pero_test_out.txt", t5);
    printf("file contents after append:\n");
    fflush(stdout);
    system("cat /tmp/pero_test_out.txt");

    /* 6. input redirection: wc -l < /tmp/pero_test_out.txt */
    Token t6[] = {
        mk(TOK_WORD, "wc"), mk(TOK_WORD, "-l"),
        mk(TOK_REDIR_IN, "<"), mk(TOK_WORD, "/tmp/pero_test_out.txt"),
        mk(TOK_EOF, NULL)
    };
    run("wc -l < /tmp/pero_test_out.txt  (expect: 2)", t6);

    /* 7. background: sleep 1 & — should return immediately, not block */
    Token t7[] = {
        mk(TOK_WORD, "sleep"), mk(TOK_WORD, "1"),
        mk(TOK_BACKGROUND, "&"), mk(TOK_EOF, NULL)
    };
    printf("\n=== sleep 1 &  (should return immediately) ===\n");
    Pipeline* p7 = parse(t7);
    printf("calling executePipeline...\n");
    fflush(stdout);
    execute(p7);
    printf("...returned immediately (not blocked for 1s)\n");
    freePipeline(p7);

    /* 8. command not found */
    Token t8[] = { mk(TOK_WORD, "this_command_does_not_exist_xyz"), mk(TOK_EOF, NULL) };
    run("this_command_does_not_exist_xyz  (expect: error + exit 127)", t8);

    // 9. pwd | grep home - testing if shell can pipe builtin commands
    Token t9[] = { 
        mk(TOK_WORD, "pwd"),
        mk(TOK_PIPE, "|"),
        mk(TOK_WORD, "grep"),
        mk(TOK_WORD, "home"),
        mk(TOK_EOF, NULL)
    };

    char cwd[1024];
    getcwd(cwd, 1024);
    run(cwd, t9);

    sleep(2); /* let the background sleep finish before the program exits */
    return 0;
}