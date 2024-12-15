#ifndef COMMON_H
    #define COMMON_H

    #define XCAT(n) #n
    #define CAT(n) XCAT(n)

    #define LENGTH_OF(arr) (sizeof (arr) / sizeof *(arr))

    #define SSTR_LENGTH(sstr) (LENGTH_OF(sstr) - 1)
    #define SSTR_UNPACK(sstr) (sstr), (SSTR_LENGTH(sstr))

#endif
