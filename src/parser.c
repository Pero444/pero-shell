#include "parser.h"

/**
 * @brief fetches the token currently stored in parser
 * @param p parser
 * @return returns current token from parser
 */
static Token* peek(Parser* p) { 
    return &p->tokens[p->pos]; 
}

/**
 * @brief returns the current token and moves to the next token in array
 * @param p parser
 * @return returns pointer to last token in array after advancing
 */
static Token* advance(Parser* p) {
    Token* current = &p->tokens[p->pos];
    if (current->type != TOK_EOF) p->pos++;

    return current;
}

/**
 * @brief checks if parsers current token type and var type match
 * @param parser parser that holds the current token
 * @param type variable we compare againts
 * @return returns 1 if values match otherwise 0
 */
static int check(Parser* p, TokenType type) { 
    return peek(p)->type == type; 
}

/* forward declaration: defined near the bottom of the file, but used by
 * parsePipeline() above it */
static Pipeline* createPipeline();

/**
 * @brief Iniitializes a simple command to default values
 * @param cmd simple command to be initialized
 */
static void simpleCommandInit(SimpleCommand* cmd) {
    cmd->argc = 0;
    cmd->argv_capacity = 8;
    cmd->argv = malloc(sizeof(char*) * cmd->argv_capacity);
    cmd->argv[0] = NULL;
    cmd->redirects = NULL;

}

/**
 * @brief Add argument to the simple command
 * @param cmd simple command 
 * @param wod argument to be added
 */
static void simple_command_add_arg(SimpleCommand* cmd, const char* word) {
    if (cmd->argc + 1 >= cmd->argv_capacity) {
        cmd->argv_capacity *= 2;
        cmd->argv = realloc(cmd->argv, sizeof(char*) * cmd->argv_capacity);
    }
    cmd->argv[cmd->argc++] = strdup(word);
    cmd->argv[cmd->argc] = NULL;

}

/**
 * @brief Add redirection to simple command
 * @param cmd simple command 
 * @param type type of redirection
 * @param filename redirection filename argument 
 */
static void simpleCommandAddRedirection(SimpleCommand* cmd, RedirType type,
                                        const char* filename) {
    Redirect* temp = malloc(sizeof(Redirect));
    temp->type = type;
    temp->filename = strdup(filename);
    temp->next = NULL;

    if (!cmd->redirects) {
        cmd->redirects = temp;
    } else {
        Redirect* tail = cmd->redirects;
        while (tail->next) tail = tail->next;
        tail->next = temp;
    }
}

/**
 * @brief Parses a single redirection operator and its filename, and attaches
 *        it to the given simple command. Does nothing and returns 0 if the
 *        current token isn't a redirection operator, so the caller can try
 *        matching something else instead.
 * @param p parser
 * @param cmd simple command the parsed redirection gets attached to
 * @return 1 if a redirection was successfully parsed and attached,
 *         0 if the current token wasn't a redirection operator at all
 */
static int parseRedirection(Parser* p, SimpleCommand* cmd) {
    RedirType type;

    // check which redirection it is
    if (check(p, TOK_REDIR_IN)) {
        type = REDIR_IN;

    } else if (check(p, TOK_REDIR_OUT)) {
        type = REDIR_OUT;

    } else if (check(p, TOK_REDIR_APPEND)) {
        type = REDIR_APPEND;

    }

    advance(p);

    if (!check(p, TOK_WORD)) {
        p->status = check(p, TOK_EOF) ? PARSE_INCOMPLETE : PARSE_ERROR;
        printf("syntax error: expected filename after redirection\n");
        return 0;
    }

    // store the filename token and a
    Token* filename_tok = advance(p);
    simpleCommandAddRedirection(cmd, type, filename_tok->value);

    return 1;
}

//

static int isRedirectionOperator(Parser* p) {
    return check(p, TOK_REDIR_APPEND) || 
           check(p, TOK_REDIR_OUT) || 
           check(p, TOK_REDIR_IN);
}

/**
 * @brief Parses one simple_command: a sequence of one or more WORDs and/or
 *        redirections in any order (grammar: simple_command := (WORD |
 *        redirection)+). Exits with a syntax error if no WORD or redirection
 *        is found at all, since a simple_command requires at least one.
 * @param p parser
 * @return the fully built SimpleCommand, returned by value
 */
static SimpleCommand parseSimpleCommand(Parser* p) {
    SimpleCommand cmd;
    // reset simple command
    simpleCommandInit(&cmd);

    int sawSomething = 0;

    while (1) {
        if (check(p, TOK_WORD)) {
            // remember last token and go to next token
            Token* temp = advance(p);
            //
            simple_command_add_arg(&cmd, temp->value);
            sawSomething = 1;
        } 
        else if (isRedirectionOperator(p)) {
            if(!parseRedirection(p, &cmd)) {
                return cmd;
            }
            sawSomething = 1;
        } 
        else {
            break;
        }
    }

    if (!sawSomething) {
        p->status = check(p, TOK_EOF) ? PARSE_INCOMPLETE : PARSE_ERROR;
    }

    return cmd;
}

/**
 * @brief Parses a full command line: one or more simple_commands chained by
 *        '|', optionally followed by a trailing '&' (grammar: pipeline :=
 *        simple_command ('|' simple_command)*, command_line := pipeline
 *        '&'?). Exits with a syntax error if any tokens remain unconsumed
 *        after the optional '&'.
 * @param p parser
 * @return pointer to the fully built Pipeline (heap-allocated; caller is
 *         responsible for freeing it, e.g. via freePipeline)
 */
static Pipeline* parsePipeline(Parser* p) {
    Pipeline* pipeline = createPipeline();

    while (1) {
        // if pipeline is longer than expected expand it
        if (pipeline->count >= pipeline->capacity) {
            pipeline->capacity *= 2;
            pipeline->commands = (SimpleCommand*)realloc(
                pipeline->commands, sizeof(SimpleCommand) * pipeline->capacity);
        }

        
        // if(!cmd.) {
        //     return
        // }
        // build the tree
        pipeline->commands[pipeline->count++] = parseSimpleCommand(p);
        if(p->status != PARSE_OK) {
            pipeline->status = p->status;
            return pipeline;
        }

        // if next token is a pipe continue parsing downstream
        if (check(p, TOK_PIPE)) {
            advance(p);
            continue;
        }
        break;
    }

    // checks if last token is a background token
    if (check(p, TOK_BACKGROUND)) {
        advance(p);
        pipeline->background = 1;
    }

    // checks for stray EOF for safety
    if (!check(p, TOK_EOF)) {
        pipeline->status = PARSE_ERROR;
        printf("Pero-shell: Stray EOF token detected.\n");
        return pipeline;
    }

    pipeline->status = PARSE_OK;
    return pipeline;
}


/**
 * @brief Allocates a new Pipeline and initializes it to default/empty
 *        values (0 commands, starting capacity of 4, background off).
 * @return pointer to the newly allocated Pipeline; caller owns this memory
 *         and must eventually free it (e.g. via freePipeline)
 */
static Pipeline* createPipeline() {
    Pipeline* temp = malloc(sizeof(Pipeline));
    temp->count = 0;
    temp->capacity = 4;
    temp->commands = malloc(sizeof(SimpleCommand) * temp->capacity);
    temp->background = 0;
    return temp;
}

/**
 * @brief Returns the printable symbol for a redirection type.
 * @param type redirection type
 * @return "<" , ">" , ">>" , or "?" if the type is unrecognized
 */
static const char* redirSymbol(RedirType type) {
    switch (type) {
        case REDIR_IN: return "<";
        case REDIR_OUT: return ">";
        case REDIR_APPEND: return ">>";
    }
    return "?";
}

/**
 * @brief Function prints the parsed tree in the terminal for debugginng
 * purposes.
 * @param p pointer to the pipeline
 */
void printPipeline(Pipeline* p) {
    printf("\nPipeline\n");
    if(p->background) {
        printf("├── run in background\n");
    }
    
    for (int i = 0; i < p->count; i++) {
        SimpleCommand* cmd = &p->commands[i];
        int isLastCmd = (i == p->count - 1);
 
        printf("%s commands[%d]\n", isLastCmd ? "└──" : "├──", i);
 
        const char* childPrefix = isLastCmd ? "    " : "│   ";
 
        // argv branch
        printf("%s├── argv:", childPrefix);
        for (int j = 0; j < cmd->argc; j++) {
            printf(" \"%s\"", cmd->argv[j]);
        }
        printf("\n");

        if (!cmd->redirects) {
            printf("%s└── redirects: (none)\n", childPrefix);
        } else {
            printf("%s└── redirects\n", childPrefix);
            for (Redirect* r = cmd->redirects; r; r = r->next) {
                int isLastRedir = (r->next == NULL);
                printf("%s    %s %s %s\n", childPrefix,
                       isLastRedir ? "└──" : "├──", redirSymbol(r->type),
                       r->filename);
            }
        }
    }
}

/**
 * @brief Frees a linked list of Redirect nodes, including each node's
 *        filename string.
 * @param r head of the redirect list to free (may be NULL)
 */
static void freeRedirects(Redirect* r) {
    while (r) {
        Redirect* next = r->next;
        free(r->filename);
        free(r);
        r = next;
    }
}

/**
 * @brief Function frees up the used memory.
 * @param p pointer to a pipeline structure to be freed
 */
void freePipeline(Pipeline* p) {
    for (int i = 0; i < p->count; i++) {
        SimpleCommand* cmd = &p->commands[i];

        for (int j = 0; j < cmd->argc; j++) {
            free(cmd->argv[j]);
        }
        free(cmd->argv);

        freeRedirects(cmd->redirects);
    }

    free(p->commands);
    free(p);
}

/**
 * @brief function parses tokens into a tree with the following grammar.
 *
 * - command_line   := pipeline '&'?
 *
 * - pipeline       := simple_command ('|' simple_command)*
 *
 * - simple_command := (WORD | redirection)+
 *
 * - redirection    := ('<' | '>' | '>>') WORD
 *
 * @param tokens array of tokens produced by the lexer, terminated by a
 *        TOK_EOF token
 * @return pointer to the fully built Pipeline; caller owns
 *         this memory and must eventually free it (e.g. via freePipeline)
 */
Pipeline* parse(Token* tokens) {
    // store tokens into parser
    // and set 0 as starting position
    Parser p = {tokens, 0, PARSE_OK};
    // return pipeline aka. parsed tree
    return parsePipeline(&p);
}