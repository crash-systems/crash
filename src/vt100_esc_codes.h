#ifndef CR_VT100_ESC_CODES_H
    #define CR_VT100_ESC_CODES_H

    #define CFMT(n) "\033[" #n "m"

    #define RESET CFMT(0)
    #define BOLD CFMT(1)

    #define RED CFMT(31)
    #define GREEN CFMT(32)
    #define YELLOW CFMT(33)
    #define BLUE CFMT(34)
    #define PURPLE CFMT(35)
    #define CYAN CFMT(36)
#endif
