#define _GNU_SOURCE

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./string.h"
#include "debug.h"
#include "env.h"

size_t count_env_entries(char **env)
{
    size_t result = 0;

    for (; *env != nullptr; env++)
        result++;
    return result;
}

static
bool parse_env_populate(char **env, buff_t *env_values,
    env_entry_t *env_entries)
{
    size_t env_size = 0;
    size_t i = 0;

    for (; *env != nullptr; env++) {
        env_size = strlen(*env);
        if (!ensure_buff_av_capacity(env_values, env_size))
            return false;
        env_entries[i].ptr = memcpy(env_values->str + env_values->count, *env,
            env_size);
        env_entries[i].size = env_size;
        env_values->count += env_size;
        i++;
    }
    return true;
}

void debug_env_entries(env_entry_t *env_entries, size_t env_size)
{
    size_t keylen[[gnu::unused]]; // Used only in debug mode

    for (size_t i = 0; i < env_size; i++) {
        keylen = strcspn(env_entries[i].ptr, "=") + 1;
        CR_DEBUG("Env entry [%01lu] key [%.*s] value [%.*s]\n", i,
            (int)keylen - 1, env_entries[i].ptr,
            (int)(env_entries[i].size - keylen),
            env_entries[i].ptr + keylen);
    }
}

bool parse_env(char **env, buff_t *env_values, env_entry_t *env_entries)
{
    if (env_values == nullptr || env_entries == nullptr)
        return false;
    memset(env_values, 0, sizeof(buff_t));
    env_values->str = malloc(sizeof *env_values->str * CR_BUFF_INIT_SZ);
    if (env_values->str == nullptr)
        return false;
    env_values->cap = CR_BUFF_INIT_SZ;
    memset(env_values->str, 0, sizeof *env_values->str * env_values->cap);
    parse_env_populate(env, env_values, env_entries);
    env_values->str[env_values->count] = '\0';
    CR_DEBUG("Parsed %zu env entries (%zu)\n",
        count_env_entries(env), env_values->count);
    return true;
}
