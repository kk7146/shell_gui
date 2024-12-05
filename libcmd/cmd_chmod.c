#include "libcmd.h"

static int apply_permissions(char *perm_str, const char *filename) {
    struct stat statbuf;
    mode_t mode;

    if (stat(filename, &statbuf) != 0) {
        perror("stat");
        return -1;
    }
    mode = statbuf.st_mode;

    if (perm_str[0] >= '0' && perm_str[0] <= '7') {
        mode_t new_mode = strtol(perm_str, NULL, 8);
        if (chmod(filename, new_mode) != 0)
            return -1;
    } else {
        //내부 로직 아직 미구현.
        if (chmod(filename, mode) != 0)
            return -1;
    }
    return 0;
}

int cmd_chmod(int argc, char **argv, int write_fd) {
    char *perm_str = NULL;
    char *file_path;
    int state = 0;

    perm_str = argv[1]; // 권한 문자열만 따로 구별.
    file_path = resolve_path(argv[2]);
    if (file_path == NULL)
        state = -1;
    if (apply_permissions(perm_str, file_path) == -1)
        state = -1;
    free(file_path);
    if (state == -1)
        write(write_fd, "e", 1);
    else
        write(write_fd, "s", 1);
    return state;
}

void usage_chmod() {
    printf("Usage: chmod [-fhv] [-R [-H | -L | -P]] mode file ... (세부 기능 아직 미구현.)\n");
}
