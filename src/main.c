#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>

#include "debug.h"

static char const PROMPT[] = "->> ";

#ifdef CR_DEBUG_MODE
int main(int argc, char **argv)
#else
int main(void)
#endif
{
    char buff[32];
    ssize_t written;
    struct termios init_settings;

    CR_DEBUG("running: %s, %d arg(s).", *argv, argc);
    tcgetattr(STDIN_FILENO, &init_settings);
    init_settings.c_iflag = IXON;
    init_settings.c_lflag = ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &init_settings);
    while (*buff != '.') {
        write(STDOUT_FILENO, PROMPT, sizeof PROMPT);
        written = read(STDIN_FILENO, &buff, sizeof buff);
        for (ssize_t i = 0; i < written; i++) {
            write(STDOUT_FILENO, &buff[i], sizeof *buff);
        }
        CR_DEBUG("read %zd bytes", written);
    }
    return EXIT_SUCCESS;
}
