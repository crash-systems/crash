#ifndef COMMAND_H_
    #define COMMAND_H_
    #include "cr_string.h"
    #include <stdbool.h>

bool command_execute(args_t *command, char **env, char **path);
args_t command_parse_args(char *buff);
#endif
