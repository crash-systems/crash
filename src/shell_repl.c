#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "./string.h"
#include "command.h"
#include "common.h"
#include "debug.h"
#include "repl.h"

char const PROMPT[] = "->> ";

#if defined CR_DEBUG_MODE
static void show_command_args(args_t *command)
{
    CR_DEBUG("command count %zu\n", command->count);
    CR_DEBUG_MSG("cmd args: ");
    for (size_t i = 0; i < command->count; i++)
        fprintf(stderr, "[%s] ", command->args[i]);
    fprintf(stderr, "\n");
}
#endif

bool shell_repl_initialize(repl_t *repl)
{
    struct termios repl_settings;

    repl->is_running = true;
    repl->input = (buff_t){ .str = nullptr, .cap = 0, .count = 0 };
    repl->is_atty = isatty(STDIN_FILENO);
    if (isatty(repl->is_atty)) {
        tcgetattr(STDIN_FILENO, &repl_settings);
        repl->saved_term_settings = repl_settings;
        repl_settings.c_iflag = IXON;
        repl_settings.c_lflag = ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &repl_settings);
    }
    return true;
}

bool shell_readline(repl_t *repl)
{
    bool succeed;

    repl->input.count = 0;
    write(STDOUT_FILENO, SSTR_UNPACK(PROMPT));
    succeed = cr_getline(&repl->input);
    CR_DEBUG("input length: [%zu]\n", repl->input.count);
    if (repl->input.count == 0) {
        repl->is_running = false;
        return false;
    }
    write(STDOUT_FILENO, "\n", 1);
    CR_DEBUG("cmd buff: [%s]\n", repl->input.str);
    return succeed;
}

bool shell_evaluate(repl_t *repl)
{
    args_t command = command_parse_args(repl->input.str);

    if (command.args == nullptr)
        return false;
    CR_DEBUG_CALL(show_command_args, &command);
    if (command.count > 0)
        if (!command_execute(repl, &command))
            repl->is_running = false;
    CR_DEBUG("cmd buff: (%zu)[%s]\n", repl->input.count, repl->input.str);
    free(command.args);
    return true;
}

bool shell_repl_run(char **env)
{
    repl_t repl = { .env = env };

    if (!shell_repl_initialize(&repl))
        return false;
    while (repl.is_running) {
        repl.input.count = 0;
        if (!shell_readline(&repl))
            continue;
        if (!shell_evaluate(&repl))
            break;
    }
    shell_cleanup(&repl);
    return (bool)!repl.is_running;
}

void shell_cleanup(repl_t *repl)
{
    if (isatty(STDIN_FILENO))
        tcsetattr(STDIN_FILENO, TCSANOW, &repl->saved_term_settings);
    free(repl->input.str);
}
