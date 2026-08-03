# pero-shell
A minimal linux shell implemented in C as a final paper project. It covers:
- tokenization of raw input
- parsing tokens into a pipeline structure
- command tree execution using fork, pipe, dup2, execvp

Parser builds a command tree using the following grammar:
- command_line   := pipeline '&'?
- pipeline       := simple_command ('|' -simple_command)*
- simple_command := (WORD | redirection)+
- redirection    := ('<' | '>' | '>>') WORD

## Input
Utilizing the gnureadline library we read user input.

## Lexer
Turns the raw input from user into tokens.

### grep foo < in.txt | sort >> out.txt

| Index | Lexeme   | Token Type         |
|------:|----------|--------------------|
| 0 | `grep`    | `TOK_WORD` |
| 1 | `foo`     | `TOK_WORD` |
| 2 | `<`       | `TOK_REDIR_IN` |
| 3 | `in.txt`  | `TOK_WORD` |
| 4 | <code>&#124;</code> | `TOK_PIPE` |
| 5 | `sort`    | `TOK_WORD` |
| 6 | `>>`      | `TOK_REDIR_APPEND` |
| 7 | `out.txt` | `TOK_WORD` |

## Parser
Parses tokens into a tree or pipeline. An array of commands in which each command holds their arguments and redirections.

### Parsed Pipeline
The command from above would be parsed like this.
```text
Pipeline
├── commands[0]
│   ├── argv: ["grep", "foo"]
│   └── redirects
│       └── < in.txt
└── commands[1]
    ├── argv: ["sort"]
    └── redirects
        └── >> out.txt
```

## Executor
Iterates over the parsed pipeline and for each command:
- creates the pipes needed to connect a multi-stage pipeline
- forks a child per command and wires stdin/stdout via dup2
- applies that command's own redirections (<, >, >>) on top of the pipe wiring, so a command's explicit redirection always takes priority over the pipe
- execvps the command, or reports "command not found" (exit 127)

## Builtins
The shell has a few builtin commands.

cd, exit, echo, env, pwd - fully implemented

export, unset - not implemented

## Prompt
shows user@host:cwd#, with the home directory abbreviated to ~ the same way bash does.

## Building & run
Run this script to build project.
```sh
./build.sh
```

Or if you want to run without rebuilding.
```sh
./run.sh
```

## Testing

The project has three tests

```sh
make lexer_test     # tokenizer edge cases (whitespace, tabs, operators)
make parser_test    # grammar edge cases, PARSE_OK/ERROR/INCOMPLETE
make executor_test  # real process execution: pipes, redirection, background
```

Still in the testing process.

## Project Structure

```text
project/
├── include/
│   ├── lexer.h
│   ├── parser.h
│   ├── executor.h
│   ├── builtins.h
│   └── ...
├── src/
│   ├── lexer.c
│   ├── parser.c
│   ├── executor.c
│   ├── builtins.c
│   ├── shell.c
│   ├── main.c
│   └── ...
├── tests/
│   ├── lexerTest.c
│   ├── parserTest.c
│   ├── executorTest.c
│   └── mini_test.h
├── build/          # Generated object files (not tracked)
└── bin/
    ├── pero
    ├── lexer_test
    ├── parser_test
    └── executor_test
```

## Known limitations / scope boundary

The following were deliberately left out of scope, given the project's
intended coverage of tokenizing, parsing, execution, redirection, and
background processes:

- **Nested subshells** `( ... )` — would require a recursive grammar
  rule (`simple_command := ... | '(' pipeline ')'`) and a matching
  recursive executor; the current grammar has no self-reference, so
  parsing stays a single left-to-right pass with no actual recursion.
- **`&&` / `||` / `;`** (AND-OR / sequential command lists)
- **Command substitution** `$(...)` and **quoting** (`'...'`, `"..."`)
- **Variable expansion** (`$HOME`, `$PATH`, `$?`) and **globbing** (`*.txt`)
- **Job control** (`jobs`, `fg`, `bg`, `Ctrl+Z`) — background processes
  are launched but not tracked in a job table
- **Signal handling** (e.g. `Ctrl+C` interrupting only the foreground
  child, not the shell itself)

These are natural next steps if the project were extended further.
