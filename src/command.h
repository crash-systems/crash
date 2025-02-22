#ifndef COMMAND_H_
    #define COMMAND_H_
    #include <stdbool.h>

    #include "repl.h"
    #include "string.h"

bool command_execute(repl_t *repl, args_t *command);

args_t command_parse_args(char *buff);
#endif
