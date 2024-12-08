#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <unistd.h>

#include "debug.h"
#include "cr_string.h"

static char const PROMPT[] = "->> ";

#define XCAT(n) #n
#define CAT(n) XCAT(n)

static
__attribute__((unused))
void show_input_buff(char *buff, size_t written)
{
    CR_DEBUG("read %zd characters:", written);
    fprintf(stderr, "[");
    for (size_t i = 0; i < written; i++) {
        if (i)
            fprintf(stderr, " ");
        fprintf(stderr, "%.2x", buff[i]);
    }
    fprintf(stderr, "]\n");
}

static
size_t strcpy_printable(char *dest, char *src)
{
    size_t count = 0;

    for (; *src != '\0'; src++) {
        if (isprint(*src)) {
            *dest = *src;
            count++;
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
    size_t read_size = 0;

    if (!ensure_capacity(buff))
        return false;
    while (*read_buff != '\n' && *read_buff != '\r') {
        read_size = read(STDOUT_FILENO, &read_buff, sizeof read_buff);
        if (!read_size)
            return false;
        CR_DEBUG_CALL(show_input_buff, read_buff, read_size);
        write(STDOUT_FILENO, read_buff, read_size);
        if (!ensure_capacity(buff))
            return false;
        buff->count += strcpy_printable(buff->str + buff->count, read_buff);
    }
    return true;
}

#ifdef CR_DEBUG_MODE
int main(int argc, char **argv)
#else
int main(void)
#endif
{
    struct termios init_settings;
    buff_t cmd_buff = { .str = NULL, 0 };

    CR_DEBUG("running: %s, %d arg(s).", *argv, argc);
    tcgetattr(STDIN_FILENO, &init_settings);
    init_settings.c_iflag = IXON;
    init_settings.c_lflag = ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &init_settings);
    write(STDOUT_FILENO, PROMPT, sizeof PROMPT);
    cr_getline(&cmd_buff);
    CR_DEBUG("cmd buff: %s", cmd_buff.str);
    write(STDOUT_FILENO, "\n", 1);
    return EXIT_SUCCESS;
}
