#ifndef UTILL_H
# define UTILL_H

# define MAX_DIR_SIZE	(128)
# define MAX_CMD_SIZE	(128)
# define BASE_DIR		"/tmp/test"

# include <stdio.h> 
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

char* resolve_path(const char *path);
int check_null_pointer(const char *ch);
int validate_path(char *path);

#endif
