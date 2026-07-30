#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "test.h"
#include "parser.h"

/* ------------------------------------------------------------------ */
/* helper: build a Token array by hand, the same way the lexer would  */
/* ------------------------------------------------------------------ */

static Token mk(TokenType type, const char* value) {
    Token t;
    t.type = type;
    t.value = value ? strdup(value) : NULL;
    return t;
}







static void run_case(const char* label, Token* tokens, parse_status expected) {
    Pipeline* p = parse(tokens);
    const char* got =
        p->status == PARSE_OK ? "OK" :
        p->status == PARSE_INCOMPLETE ? "INCOMPLETE" : "ERROR";
    const char* want =
        expected == PARSE_OK ? "OK" :
        expected == PARSE_INCOMPLETE ? "INCOMPLETE" : "ERROR";
 
    printf("%-45s expected=%-10s got=%-10s %s\n", label, want, got,
           p->status == expected ? "PASS" : "FAIL");
    freePipeline(p);
}








/**
 * @brief Frees the .value strings strdup'd by mk() for a stack-allocated
 * mock token array. Does NOT free `tokens` itself — it's a local array,
 * not a malloc'd block, so freeing the array pointer would be invalid.
 * parse() makes its own copies of these strings into the AST it builds,
 * so it never frees the originals — that's on whoever built the mock
 * array (us, here), same as freeTokens() would do for real lexer output.
 */
static void free_mock_tokens(Token* tokens) {
    for (int i = 0; tokens[i].type != TOK_EOF; i++) {
        free(tokens[i].value);
    }
}

/* ------------------------------------------------------------------ */
/* helper: run parse() in a forked child, since bad input makes it     */
/* call exit(2) — doing that in THIS process would kill the whole      */
/* test binary before later tests get to run.                          */
/* ------------------------------------------------------------------ */
static int parse_exits_with_code(Token* tokens, int expected_code) {
    fflush(NULL); /* avoid duplicated output from the child's inherited,
                     unflushed stdio buffer — see earlier fork+stdio note */

    pid_t pid = fork();

    if (pid == 0) {
        freopen("/dev/null", "w", stderr); /* silence the expected error msg */
        parse(tokens);                      /* should exit(2) and never return */
        exit(0);                            /* only reached if it's a bug */
    }

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && WEXITSTATUS(status) == expected_code;
}

/* ------------------------------------------------------------------ */
/* Test: two pipes in a row with nothing between them — "cmd | | cmd2" */
/* ------------------------------------------------------------------ */
static void test_consecutive_pipes_error(void) {
    Token tokens[] = {
        mk(TOK_WORD, "cmd"),
        mk(TOK_PIPE, "|"),
        mk(TOK_PIPE, "|"),
        mk(TOK_WORD, "cmd2"),
        mk(TOK_EOF, NULL)
    };

    /* after the first '|', parseSimpleCommand immediately sees another
     * pipe token instead of a WORD/redirection, so "saw_something" stays
     * false and it should hit the "expected command" syntax error */
    ASSERT_TRUE(parse_exits_with_code(tokens, 2));

    free_mock_tokens(tokens);
}

/* ------------------------------------------------------------------ */
/* Test: trailing pipe with nothing after it — "cmd |"                 */
/* ------------------------------------------------------------------ */
static void test_trailing_pipe_error(void) {
    Token tokens[] = {
        mk(TOK_WORD, "cmd"),
        mk(TOK_PIPE, "|"),
        mk(TOK_EOF, NULL)
    };

    /* the pipe consumes and expects another simple_command, but only
     * TOK_EOF follows — same "expected command" error path */
    ASSERT_TRUE(parse_exits_with_code(tokens, 2));

    free_mock_tokens(tokens);
}

/* ------------------------------------------------------------------ */
/* Test: two redirection operators in a row — "cmd > > out.txt"        */
/* ------------------------------------------------------------------ */
static void test_consecutive_redirections_error(void) {
    Token tokens[] = {
        mk(TOK_WORD, "cmd"),
        mk(TOK_REDIR_OUT, ">"),
        mk(TOK_REDIR_OUT, ">"),
        mk(TOK_WORD, "out.txt"),
        mk(TOK_EOF, NULL)
    };

    /* after consuming the first '>', parseRedirection requires the very
     * next token to be TOK_WORD (the filename); seeing another '>'
     * instead should trigger "expected filename after redirection" */
    ASSERT_TRUE(parse_exits_with_code(tokens, 2));

    free_mock_tokens(tokens);
}

/* ------------------------------------------------------------------ */
/* Test: multiple redirections on one command preserve their order,    */
/* including when the SAME operator repeats ("cmd > a > b" — last one   */
/* should conceptually win at exec time, but the parser must still      */
/* store BOTH in the order they appeared, not silently drop one)         */
/* ------------------------------------------------------------------ */
static void test_repeated_redirect_type_preserves_order(void) {
    Token tokens[] = {
        mk(TOK_WORD, "cmd"),
        mk(TOK_REDIR_OUT, ">"),
        mk(TOK_WORD, "a.txt"),
        mk(TOK_REDIR_OUT, ">"),
        mk(TOK_WORD, "b.txt"),
        mk(TOK_EOF, NULL)
    };

    Pipeline* p = parse(tokens);
    Redirect* r = p->commands[0].redirects;

    ASSERT_TRUE(r != NULL);
    ASSERT_STR_EQ("a.txt", r->filename);
    ASSERT_TRUE(r->next != NULL);
    ASSERT_STR_EQ("b.txt", r->next->filename);
    ASSERT_TRUE(r->next->next == NULL);

    freePipeline(p);
    free_mock_tokens(tokens);
}

int main(void) {
    //RUN_TEST(test_consecutive_pipes_error);
    //RUN_TEST(test_trailing_pipe_error);
    //RUN_TEST(test_consecutive_redirections_error);
    //RUN_TEST(test_repeated_redirect_type_preserves_order);






        /* "ls -la"  -> normal, complete command */
    Token t1[] = { mk(TOK_WORD, "ls"), mk(TOK_WORD, "-la"), mk(TOK_EOF, NULL) };
    run_case("ls -la", t1, PARSE_OK);
 
    /* "ls |"  -> trailing pipe, nothing after: bash waits (PS2) */
    Token t2[] = { mk(TOK_WORD, "ls"), mk(TOK_PIPE, "|"), mk(TOK_EOF, NULL) };
    run_case("ls |", t2, PARSE_INCOMPLETE);
 
    /* "|"  -> leading pipe, nothing before: bash errors immediately */
    Token t3[] = { mk(TOK_PIPE, "|"), mk(TOK_EOF, NULL) };
    run_case("| (lone pipe)", t3, PARSE_ERROR);
 
    /* "sort >"  -> redirection with no filename yet: bash waits (PS2) */
    Token t4[] = { mk(TOK_WORD, "sort"), mk(TOK_REDIR_OUT, ">"), mk(TOK_EOF, NULL) };
    run_case("sort >", t4, PARSE_INCOMPLETE);
 
    /* "sort > |"  -> operator followed by ANOTHER operator: real error */
    Token t5[] = { mk(TOK_WORD, "sort"), mk(TOK_REDIR_OUT, ">"),
                   mk(TOK_PIPE, "|"), mk(TOK_EOF, NULL) };
    run_case("sort > |", t5, PARSE_ERROR);
 
    /* "ls | wc -l"  -> normal two-stage pipeline */
    Token t6[] = { mk(TOK_WORD, "ls"), mk(TOK_PIPE, "|"),
                   mk(TOK_WORD, "wc"), mk(TOK_WORD, "-l"), mk(TOK_EOF, NULL) };
    run_case("ls | wc -l", t6, PARSE_OK);
 
    /* "cmd | |"  -> two pipes in a row: real error, not incomplete */
    Token t7[] = { mk(TOK_WORD, "cmd"), mk(TOK_PIPE, "|"),
                   mk(TOK_PIPE, "|"), mk(TOK_EOF, NULL) };
    run_case("cmd | |", t7, PARSE_ERROR);






    return test_summary();
}