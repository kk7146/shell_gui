#ifndef CUSTOMSHELL_H
# define CUSTOMSHELL_H

#define MAX_ARG             (20)

# include "commandmanager.h"

int init();
int execute_shell(char *command, cmd_node *const head, int read_fd, int write_fd);

#endif