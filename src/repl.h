#ifndef CR_REPL_H
    #define CR_REPL_H

    #include <stdbool.h>
    #include <termios.h>

    #include "string.h"

typedef struct {
    buff_t input;
    int is_atty;
    bool is_running;
    char **env;
    struct termios saved_term_settings;
    int status;
} repl_t;

extern char const PROMPT[];

bool shell_repl_initialize(repl_t *repl);
void shell_cleanup(repl_t *repl);
bool shell_readline(repl_t *repl);
bool shell_evaluate(repl_t *repl);

bool shell_repl_run(char **env);

#endif
