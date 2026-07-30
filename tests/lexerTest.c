#include <stdio.h>

#include "lexer.h"
#include "test.h"

/**
 * @brief Function build a token structure with given data
 * @param type type of token
 * @param value value of token
 * @return Returns the built struct data
 */
static Token mk(TokenType type, const char* value) {
    Token t;
    t.type = type;
    t.value = value ? strdup(value) : NULL;
    return t;
}

/**
 * @brief single-token lexer smoke test
 */
static void test_single_token(void) {
    char line[] = "grep";
    Token* tokens = lexer(line);

    ASSERT_STR_EQ("grep", tokens[0].value);

    freeTokens(tokens); /* was: freeTokens() — missing the pointer arg */
}

/**
 * @brief multi-token lexer test covering words, redirection operators,
 * pipe, and filenames all in one line
 */
static void test_multiple_tokens(void) { /* was: test_multipe_tokens (typo) */
    char line[] = "grep foo < in.txt | sort >> out.txt";
    Token* tokens = lexer(line);

    ASSERT_STR_EQ("grep", tokens[0].value);
    ASSERT_STR_EQ("foo", tokens[1].value);
    ASSERT_STR_EQ("<", tokens[2].value);
    ASSERT_STR_EQ("in.txt", tokens[3].value);
    ASSERT_STR_EQ("|", tokens[4].value);
    ASSERT_STR_EQ("sort", tokens[5].value);       /* was tokens[0] */
    ASSERT_STR_EQ(">>", tokens[6].value);          /* was tokens[0] */
    ASSERT_STR_EQ("out.txt", tokens[7].value);     /* was tokens[0] */

    freeTokens(tokens);
}

/**
 * @brief empty input should still produce a valid, non-NULL token array
 * whose only entry is the TOK_EOF terminator — not a NULL pointer, and
 * not a crash.
 */
static void test_empty_input(void) {
    char line[] = "";
    Token* tokens = lexer(line);

    ASSERT_TRUE(tokens != NULL);
    ASSERT_EQ_INT(TOK_EOF, tokens[0].type);

    freeTokens(tokens);
}

/**
 * @brief runs of multiple spaces/tabs between words should collapse down
 * to the same token count as single-space-separated input — strtok's
 * delimiter-collapsing behavior, tested explicitly rather than assumed.
 */
static void test_multiple_spaces_collapse(void) {
    char line[] = "grep    foo";
    Token* tokens = lexer(line);

    ASSERT_STR_EQ("grep", tokens[0].value);
    ASSERT_STR_EQ("foo", tokens[1].value);
    ASSERT_EQ_INT(TOK_EOF, tokens[2].type); /* nothing extra from the gaps */

    freeTokens(tokens);
}

/**
 * @brief leading and trailing whitespace shouldn't produce phantom empty
 * tokens at the start or end of the array.
 */
static void test_leading_trailing_spaces(void) {
    char line[] = "  grep foo  ";
    Token* tokens = lexer(line);

    ASSERT_STR_EQ("grep", tokens[0].value);
    ASSERT_STR_EQ("foo", tokens[1].value);
    ASSERT_EQ_INT(TOK_EOF, tokens[2].type);

    freeTokens(tokens);
}

/**
 * @brief each operator, alone with no surrounding words, should still be
 * classified correctly rather than accidentally falling through to
 * TOK_WORD (the isType() fallback case).
 */
static void test_lone_operators(void) {
    char pipe_line[] = "|";
    Token* t1 = lexer(pipe_line);
    ASSERT_EQ_INT(TOK_PIPE, t1[0].type);
    freeTokens(t1);

    char in_line[] = "<";
    Token* t2 = lexer(in_line);
    ASSERT_EQ_INT(TOK_REDIR_IN, t2[0].type);
    freeTokens(t2);

    char out_line[] = ">";
    Token* t3 = lexer(out_line);
    ASSERT_EQ_INT(TOK_REDIR_OUT, t3[0].type);
    freeTokens(t3);

    char append_line[] = ">>";
    Token* t4 = lexer(append_line);
    ASSERT_EQ_INT(TOK_REDIR_APPEND, t4[0].type);
    freeTokens(t4);

    char bg_line[] = "&";
    Token* t5 = lexer(bg_line);
    ASSERT_EQ_INT(TOK_BACKGROUND, t5[0].type);
    freeTokens(t5);
}

/**
 * @brief tabs must be treated as delimiters just like spaces, since
 * strtok(temp, " \t") lists both — testing this explicitly rather than
 * assuming tabs behave the same as spaces just because spaces do.
 */
static void test_tabs_as_whitespace(void) {
    char line[] = "grep\tfoo";
    Token* tokens = lexer(line);

    ASSERT_STR_EQ("grep", tokens[0].value);
    ASSERT_STR_EQ("foo", tokens[1].value);
    ASSERT_EQ_INT(TOK_EOF, tokens[2].type);

    freeTokens(tokens);
}

/**
 * @brief a line containing ONLY whitespace (no words at all) is subtly
 * different from a truly empty string ("") — confirm it's handled the
 * same way (just the TOK_EOF terminator), not as some malformed token.
 */
static void test_whitespace_only_input(void) {
    char line[] = "    ";
    Token* tokens = lexer(line);

    ASSERT_TRUE(tokens != NULL);
    ASSERT_EQ_INT(TOK_EOF, tokens[0].type);

    freeTokens(tokens);
}

/**
 * @brief a single very long argument shouldn't be truncated or corrupted —
 * guards against any hidden fixed-size buffer assumption in the lexer.
 */
static void test_long_word(void) {
    char long_word[600];
    memset(long_word, 'a', sizeof(long_word) - 1);
    long_word[sizeof(long_word) - 1] = '\0';

    char line[700];
    snprintf(line, sizeof(line), "%s", long_word);

    Token* tokens = lexer(line);

    ASSERT_STR_EQ(long_word, tokens[0].value);
    ASSERT_EQ_INT((long)strlen(long_word), (long)strlen(tokens[0].value));

    freeTokens(tokens);
}

/**
 * @brief DOCUMENTS current, known behavior rather than testing a
 * requirement: since this lexer is space-required (operators glued
 * directly to words are NOT split apart — a deliberate scope decision,
 * not a bug), "ls|grep" is expected to come back as ONE single TOK_WORD
 * token containing the literal text "ls|grep", not three separate
 * tokens. If this test ever starts failing, it means the lexer's
 * space-required assumption changed, which is worth knowing on purpose.
 */
static void test_operator_without_spaces_is_one_word(void) {
    char line[] = "ls|grep";
    Token* tokens = lexer(line);

    ASSERT_EQ_INT(TOK_WORD, tokens[0].type);
    ASSERT_STR_EQ("ls|grep", tokens[0].value);

    freeTokens(tokens);
}

int main(void) {
    // lexer tests
    RUN_TEST(test_single_token);
    RUN_TEST(test_multiple_tokens);
    RUN_TEST(test_empty_input);
    RUN_TEST(test_multiple_spaces_collapse);
    RUN_TEST(test_leading_trailing_spaces);
    RUN_TEST(test_lone_operators);
    RUN_TEST(test_tabs_as_whitespace);
    RUN_TEST(test_whitespace_only_input);
    RUN_TEST(test_long_word);
    RUN_TEST(test_operator_without_spaces_is_one_word);
    
    return test_summary();
}