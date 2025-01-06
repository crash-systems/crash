#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "env.h"
#include "string.h"

buff_t *parse_env(char **env, buff_t *result)
{
    size_t env_size = 0;

    if (result == NULL)
        return NULL;
    bzero(result, sizeof(buff_t));
    for (; *env != NULL; env++) {
        env_size = strlen(*env);
        if (!ensure_buff_av_capacity(result, env_size))
            return NULL;
        memcpy(result->str + result->count, *env, env_size);
        result->count += env_size;
    }
    result->str[result->count] = '\0';
    CR_DEBUG("Parsed env: %s\n", result->str);
    return result;
}
