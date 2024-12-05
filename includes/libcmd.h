#ifndef LIBCMD_H
# define LIBCMD_H

# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <dirent.h>
# include <pwd.h>
# include <grp.h>
# include <time.h>
# include <signal.h>
# include <errno.h>
# include <stdlib.h>
# include "util.h"

# define PID_SIZE 32768
# define MAX_CMD_NAME 10

# define DECLARE_CMDFUNC(str)   int cmd_##str(int argc, char **argv, int write_fd); \
                                void usage_##str(void)

DECLARE_CMDFUNC(ls);
DECLARE_CMDFUNC(chmod);
DECLARE_CMDFUNC(cat);
DECLARE_CMDFUNC(cd);
DECLARE_CMDFUNC(mkdir);
DECLARE_CMDFUNC(rmdir);
DECLARE_CMDFUNC(rename);
DECLARE_CMDFUNC(ln);
DECLARE_CMDFUNC(rm);
DECLARE_CMDFUNC(cp);
DECLARE_CMDFUNC(ps);
DECLARE_CMDFUNC(kill);
DECLARE_CMDFUNC(exec);

typedef int  (*cmd_func_t)(int argc, char **argv, int write_fd);
typedef void (*usage_func_t)(void);

typedef struct cmd_list {
    char name[MAX_CMD_NAME];
    cmd_func_t cmd_func;
    usage_func_t usage_func;
} cmd_list;

static cmd_list list[] = {
    {"ls",      cmd_ls,      usage_ls},
    {"chmod",   cmd_chmod,   usage_chmod},
    {"cat",     cmd_cat,     usage_cat},
    {"cd",      cmd_cd,      usage_cd},
    {"mkdir",   cmd_mkdir,   usage_mkdir},
    {"rmdir",   cmd_rmdir,   usage_rmdir},
    {"rename",  cmd_rename,  usage_rename},
    {"ln",      cmd_ln,      usage_ln},
    {"rm",      cmd_rm,      usage_rm},
    {"cp",      cmd_cp,      usage_cp},
    {"ps",      cmd_ps,      usage_ps},
    {"kill",    cmd_kill,    usage_kill},
    {"exec",    cmd_exec,    usage_exec}
};

#endif
