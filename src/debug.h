#ifndef CR_DEBUG_H
    #define CR_DEBUG_H

    #include <stdio.h> /* IWYU pragma: export */

    #include "vt100_esc_codes.h"

    #define OMIT /* omitted */

    #ifdef CR_DEBUG_MODE
        #define HEAD __FILE_NAME__, __LINE__

        #define HEAD_FMT_FILE BOLD BLUE "%s" RESET
        #define HEAD_FMT_LINE ":" BOLD PURPLE "%d" RESET

        #define HEAD_FMT HEAD_FMT_FILE HEAD_FMT_LINE "  "

        #define ERR(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
        #define DEBUG_INTERNAL(fmt, ...) ERR(HEAD_FMT fmt, HEAD, __VA_ARGS__)

        #define CR_DEBUG(fmt, ...) DEBUG_INTERNAL(fmt, __VA_ARGS__)
        #define CR_DEBUG_MSG(msg) DEBUG_INTERNAL("%s", msg)

        #define CR_DEBUG_USED OMIT
        #define CR_DEBUG_CALL(func, ...) func(__VA_ARGS__)

    #else

        #define CR_DEBUG_MSG(msg) OMIT
        #define CR_DEBUG(fmt, ...) OMIT

        #define CR_DEBUG_CALL(func, ...) OMIT
        #define CR_DEBUG_USED __attribute__((unused))

        #define DEBUG_SINK_FD OMIT
        #define DEBUG_SINK -1
    #endif
#endif
