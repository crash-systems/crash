#ifndef CR_DEBUG_H
    #define CR_DEBUG_H

    #include "vt100_esc_codes.h"

    #ifdef CR_DEBUG_MODE
        #define HEAD __FILE_NAME__, __LINE__

        #define HEAD_FMT_FILE BOLD BLUE "%s" RESET
        #define HEAD_FMT_LINE ":" BOLD PURPLE "%d" RESET

        #define HEAD_FMT HEAD_FMT_FILE HEAD_FMT_LINE "  "

        #define CR_DEBUG_MSG(msg) CR_DEBUG("%s", ...)
        #define CR_DEBUG(fmt, ...) printf(HEAD_FMT fmt "\n", HEAD, __VA_ARGS__)

        #define CR_DEBUG_CALL(func, ...) func(__VA_ARGS__)
    #else
        #define OMIT /* omitted */

        #define CR_DEBUG_MSG(msg) OMIT
        #define CR_DEBUG(fmt, ...) OMIT

        #define CR_DEBUG_CALL(func, ...) OMIT
    #endif

#endif
