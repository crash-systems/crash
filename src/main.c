#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

#include "command.h"
#include "common.h"
#include "cr_string.h"
#include "debug.h"

static char const PROMPT[] = "->> ";

#if defined CR_DEBUG_MODE
static void show_input_buff(char const buff[static 1], size_t written)
{
    CR_DEBUG("read %zd characters: [", written);
    for (size_t i = 0; i < written; i++) {
        if (i)
            dprintf(STDERR_FILENO, " ");
        dprintf(STDERR_FILENO, "%.2x", buff[i]);
    }
    dprintf(STDERR_FILENO, "]\n");
    if (*buff == '\033')
        CR_DEBUG("Detected ascii sequence: [\\033%s]\n", &buff[1]);
}

static void show_command_args(args_t *command)
{
    CR_DEBUG("command count %zu\n", command->count);
    CR_DEBUG_MSG("cmd args: ");
    for (size_t i = 0; i < command->count; i++)
        dprintf(STDERR_FILENO, "[%s] ", command->args[i]);
    dprintf(STDERR_FILENO, "\n");
}
#endif

static
size_t strcpy_printable(char *dest, char *src)
{
    size_t count = 0;

    for (; *src != '\0'; src++) {
        if (isprint(*src)) {
            *dest = *src;
            count++;
            dest++;
        }
    }
    return count;
}

static
bool ensure_capacity(buff_t *buff)
{
    char *new_str;

    if (!buff->str) {
        new_str = malloc((sizeof *buff->str) * CR_BUFF_INIT_SZ);
        if (new_str == NULL)
            return false;
        buff->str = new_str;
        buff->cap = CR_BUFF_INIT_SZ;
    }
    if (buff->count == buff->cap) {
        new_str = realloc(buff->str, (sizeof *buff->str) * buff->cap << 1);
        if (new_str == NULL)
            return false;
        buff->str = new_str;
        buff->cap <<= 1;
    }
    return true;
}

static
bool cr_getline(buff_t *buff)
{
    char read_buff[32] = "";
    ssize_t read_size = 0;

    if (!ensure_capacity(buff))
        return false;
    while (*read_buff != '\n' && *read_buff != '\r') {
        bzero(read_buff, sizeof read_buff);
        read_size = read(STDIN_FILENO, &read_buff, sizeof read_buff);
        if (read_size <= 0)
            return (bool)(!read_size);
        if (*read_buff == CTRL('d'))
            return write(STDOUT_FILENO, SSTR_UNPACK("exit")), true;
        CR_DEBUG_CALL(show_input_buff, read_buff, read_size);
        write(STDOUT_FILENO, read_buff, read_size);
        if (!ensure_capacity(buff))
            return false;
        buff->count += strcpy_printable(buff->str + buff->count, read_buff);
    }
    return true;
}

int main(int argc CR_DEBUG_USED, char **argv CR_DEBUG_USED, char **env)
{
    struct termios base_settings;
    struct termios repl_settings;
    buff_t cmd_buff = { .str = NULL, 0 };
    args_t command;
    bool succeed;

    CR_DEBUG("running: %s, %d arg(s).\n", *argv, argc);
    if (isatty(STDIN_FILENO)) {
        tcgetattr(STDIN_FILENO, &base_settings);
        repl_settings = base_settings;
        repl_settings.c_iflag = IXON;
        repl_settings.c_lflag = ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSANOW, &repl_settings);
    }
    write(STDOUT_FILENO, PROMPT, sizeof PROMPT);
    succeed = cr_getline(&cmd_buff);
    write(STDOUT_FILENO, "\n", 1);
    CR_DEBUG("cmd buff: [%s]\n", cmd_buff.str);
    command = command_parse_args(cmd_buff.str);
    if (command.args == NULL)
        return EXIT_FAILURE;
    CR_DEBUG_CALL(show_command_args, &command);
    command_execute(&command, env, NULL);
    CR_DEBUG("cmd buff: (%zu)[%s]\n", cmd_buff.count, cmd_buff.str);
    free(command.args);
    if (isatty(STDIN_FILENO))
        tcsetattr(STDIN_FILENO, TCSANOW, &base_settings);
    return (!succeed) ? EXIT_FAILURE : EXIT_SUCCESS;
}
