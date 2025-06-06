#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "string.h"

bool ensure_buff_capacity(buff_t *buff)
{
    char *new_str;

    if (buff->str == nullptr) {
        new_str = malloc((sizeof *buff->str) * CR_BUFF_INIT_SZ);
        if (new_str == nullptr)
            return false;
        buff->str = new_str;
        buff->cap = CR_BUFF_INIT_SZ;
    }
    if (buff->count == buff->cap) {
        new_str = realloc(buff->str, (sizeof *buff->str) * buff->cap << 1);
        if (new_str == nullptr)
            return false;
        buff->str = new_str;
        buff->cap <<= 1;
    }
    return true;
}

bool ensure_buff_av_capacity(buff_t *buff, size_t requested)
{
    char *new_str;
    size_t endsize = CR_BUFF_INIT_SZ;

    if ((buff->count + requested) < buff->cap)
        return true;
    for (; endsize < buff->count + requested; endsize <<= 1);
    if (endsize > buff->cap) {
        new_str = realloc(buff->str, (sizeof *buff->str) * endsize);
        if (new_str == nullptr)
            return false;
        buff->str = new_str;
        buff->cap = endsize;
    }
    return true;
}

bool str_printable(char const *str, size_t size)
{
    for (size_t i = 0; i < size; i++)
        if (!isprint(str[i]))
            return false;
    return true;
}
