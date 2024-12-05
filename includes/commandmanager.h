#ifndef COMMANDMANAGER_H
# define COMMANDMANAGER_H

# include "libcmd.h"

typedef struct cmd_node {
    char cmd_str[MAX_CMD_SIZE];
    cmd_func_t cmd_func;
    usage_func_t usage_func;
    struct cmd_node *next;
} cmd_node;

void add_command(cmd_node **head, char *name, cmd_func_t cmd, usage_func_t usage);
void remove_command(cmd_node **head, char *name);
cmd_node* find_command(cmd_node *head, char *name);
int execute_command(cmd_node *head, char *cmd, int argc, char **argv);
void free_all_commands(cmd_node **head);
void help(cmd_node *head);

#endif
