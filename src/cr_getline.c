#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "./string.h"
#include "common.h"
#include "debug.h"
#include "repl.h"

#if defined(CR_DEBUG_MODE)
    #include <stdio.h>
    #include <sys/ioctl.h>
#endif

#define ctrl(x) (x & 0xf)

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
bool append_null_terminator(buff_t *buff)
{
    if (!ensure_buff_capacity(buff))
        return false;
    buff->str[buff->count] = '\0';
    buff->count++;
    return true;
}

bool cr_getline(buff_t *buff)
{
    char read_buff[32] = "";
    ssize_t read_size = 0;

    if (!ensure_buff_capacity(buff))
        return false;
    while (*read_buff != '\n' && *read_buff != '\r') {
        memset(read_buff, '\0', sizeof read_buff);
        read_size = read(STDIN_FILENO, &read_buff, sizeof read_buff);
#if defined(CR_DEBUG_MODE)
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);

        size_t erase = strlen(PROMPT) + buff->count;
        size_t line_count = erase / w.ws_col;

        // delete the prompt to allow proper debug logs
        if (erase > w.ws_col)
            fprintf(stderr, "\033[%zuA", line_count);
        fprintf(stderr, "\033[0G\033[0J");
        CR_DEBUG("read count: %zd\n", read_size);
#endif
        if (read_size < 0)
            return false;
        if (read_size == 0) {
            buff->count = 0;
            return true;
        }
        if (*read_buff == ctrl('d'))
            return write(STDOUT_FILENO, SSTR_UNPACK("exit\n")), true;
#if defined(CR_DEBUG_MODE)
        CR_DEBUG("read %zd characters: [", read_size);
        for (ssize_t i = 0; i < read_size; i++) {
            if (i)
                fprintf(stderr, " ");
            fprintf(stderr, "%.2x", read_buff[i]);
        }
        fprintf(stderr, "]\n");
        if (*read_buff == '\033')
            CR_DEBUG("Detected ascii sequence: [\\033%s]\n", &read_buff[1]);

        // print back the pompt
        fprintf(stderr, "%s%.*s", PROMPT, (int)buff->count, buff->str);
#endif
        if (str_printable(read_buff))
            write(STDOUT_FILENO, read_buff, read_size);
        if (!ensure_buff_capacity(buff))
            return false;
        buff->count += strcpy_printable(buff->str + buff->count, read_buff);
    }
    CR_DEBUG("buff count: %zu\n", buff->count);
    return append_null_terminator(buff);
}
