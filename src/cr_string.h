#ifndef CR_STRING_H
    #define CR_STRING_H
    #include <stddef.h>
    #define CR_BUFF_INIT_SZ 128

typedef struct {
    char *str;
    size_t count;
    size_t cap;
} buff_t;

typedef struct {
    char **args;
    size_t count;
    size_t cap;
} args_t;

bool cr_getline(buff_t *buff);

#endif
