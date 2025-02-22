#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/limits.h>

#include "command.h"
#include "common.h"
#include "debug.h"
#include "env.h"
#include "repl.h"

static
int ensure_args_capacity(args_t *command)
{
    char **ptr = NULL;

    if (command->count == command->cap - 1) {
        ptr = realloc(command->args, sizeof *command * command->cap << 1);
        if (ptr == NULL)
            return 0;
        command->args = ptr;
        command->cap <<= 1;
        command->args[command->count] = NULL;
    }
    return command->cap;
}

args_t command_parse_args(char *buff)
{
    args_t command = { 0, .cap = 15, .count = 0 };

    command.args = malloc(sizeof *command.args * command.cap);
    if (command.args == NULL)
        return (args_t){ .args = NULL };
    for (size_t i = command.count; i < command.cap; i++)
        command.args[i] = NULL;
    for (char *tok = strtok(buff, " "); tok != NULL; tok = strtok(NULL, " ")) {
        command.cap = ensure_args_capacity(&command);
        if (!command.cap)
            return free(command.args), (args_t){ NULL, 0, 0 };
        command.args[command.count] = tok;
        command.count++;
    }
    CR_DEBUG("Command: [%s] with %zu args\n",
        command.args[0], command.count);
    return command;
}

static
void command_handler_errors(char const *name)
{
    switch (errno) {
        case ENOENT:
            dprintf(STDERR_FILENO, "%s: Command not found.\n", name);
            break;
        default:
            dprintf(STDERR_FILENO, "execve: Unknown error\n");
    }
}

static
bool execute_env(char **env)
{
    buff_t env_values = { 0 };
    env_entry_t *env_entries = NULL;
    size_t env_size = count_env_entries(env);

    env_values.str = malloc(CR_BUFF_INIT_SZ);
    if (env_values.str == NULL)
        return false;
    env_entries = malloc(sizeof *env_entries * env_size);
    parse_env(env, &env_values, env_entries);
    CR_DEBUG_CALL(debug_env_entries, env_entries, env_size);
    free(env_values.str);
    free(env_entries);
    return true;
}

static
void shell_command_show_signal(int rstatus)
{
    if (!WIFEXITED(rstatus) && WIFSIGNALED(rstatus)) {
        if (WTERMSIG(rstatus) != W_STOPCODE(rstatus))
            dprintf(STDERR_FILENO, "%s", strsignal(WTERMSIG(rstatus)));
        else
            write(STDERR_FILENO, SSTR_UNPACK("Floating exception"));
        if (WCOREDUMP(rstatus))
            write(STDERR_FILENO, SSTR_UNPACK(" (core dumped)"));
        write(STDERR_FILENO, SSTR_UNPACK("\n"));
    }
}

static
bool shell_command_run_subprocess(repl_t *repl, args_t *command, char const *path)
{
    pid_t pid = fork();

    if (pid < 0)
        return -1;
    CR_DEBUG("PID [%d | %d]", pid, getpid());
    if (pid) {
        waitpid(pid, &repl->status, 0);
        CR_DEBUG("Full exit status %d", repl->status);
        shell_command_show_signal(repl->status);
        repl->status = WEXITSTATUS(repl->status);
        return true;
    }
    execve(path, command->args, repl->env);
    command_handler_errors("crash");
    return true;
}

static
char *path_append(char *absp, char const *leaf)
{
    size_t len = strlen(absp);
    size_t leaf_len = strlen(leaf);

    if (absp[len - 1] == '/')
        len--;
    if ((PATH_MAX - len) <= len)
        return NULL;
    absp[len] = '/';
    memcpy(absp + len + 1, leaf, (leaf_len + 1) * sizeof(char));
    return absp;
}


static
char *search_in_env_path(char **env, char *path, const char *filename)
{
    size_t len;
    const char *search;

    // TODO: use a env retriver
    for (; strncmp(*env, "PATH=", 5) != 0; env++);
    search = *env;

    for (; search != NULL;) {
        for (; *search == ':'; search++);
        if (*search == '\0')
            return NULL;
        len = strcspn(search, ":");
        CR_DEBUG("search: [%.*s]", (int)len, search);
        memcpy(path, search, len * sizeof(char));
        path[len] = '\0';
        path_append(path, filename);
        if (!access(path, F_OK))
            return path;
        search = strchr(search, ':');
    }
    return NULL;
}

static
int stridx(const char *str, char c)
{
    for (const char *p = str; *p != '\0'; p++)
        if (*p == c)
            return p - str;
    return -1;
}

static
char const *path_resolve(char *cmdpath, char const *cmd, char **env)
{
    if (*cmd == '/')
        return cmd;
    if (!strncmp(cmd, "./", 2)) {
       if (getcwd(cmdpath, PATH_MAX) != NULL)
            return path_append(cmdpath, cmd);
    }
    if (search_in_env_path(env, cmdpath, cmd) != NULL)
        return cmdpath;
    if (getcwd(cmdpath, PATH_MAX) == NULL)
        return NULL;
    if (stridx(cmd, '/') != -1)
        return path_append(cmdpath, cmd);
    return NULL;
}

bool command_execute(repl_t *repl, args_t *command)
{
    char buff[PATH_MAX];
    char const *cmdpath;

    if (command->args[0] == NULL)
        return false;
    CR_DEBUG("argv[0] -> [%s]", command->args[0]);
    if (strcmp(command->args[0], "env") == 0)
        return execute_env(repl->env);
    cmdpath = path_resolve(buff, command->args[0], repl->env);
    return shell_command_run_subprocess(repl, command, cmdpath);
}
