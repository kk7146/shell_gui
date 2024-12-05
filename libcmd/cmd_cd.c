#include "libcmd.h"

int cmd_cd(int argc, char **argv, int write_fd) { // cd 명령어 구현부
    char * new_path;

    if (argc > 2)
    {
        printf("cd: too many argument\n");
        write(write_fd, "e", 2);
        return -2;
    }
    if (argc == 1)
    {
        if (chdir(BASE_DIR) != 0)
        {
            perror("cd");
            write(write_fd, "e", 2);
            return -1;
        }
        write(write_fd, "e", 2);
        return 0;
    }
    new_path = resolve_path(argv[1]);
    if (chdir(new_path) != 0)
    {
        perror("cd");
        write(write_fd, "e", 2);
        free(new_path);
        return -1;
    }
    free(new_path);
    write(write_fd, "s", 2);
    return 0;
}

void usage_cd() {
    printf("cd <path>              : Change directory to <path>\n");
}
