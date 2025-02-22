#include <stdbool.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "debug.h"
#include "repl.h"

int main(int argc CR_DEBUG_USED, char **argv CR_DEBUG_USED, char **env)
{
    CR_DEBUG("running: %s, %d arg(s).\n", *argv, argc);
    return !shell_repl_run(env) ? EXIT_FAILURE : EXIT_SUCCESS;
}
