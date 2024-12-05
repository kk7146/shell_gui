#include "libcmd.h"
#include <libgen.h>

int cmd_rmdir(int argc, char **argv, int write_fd) {
    char *new_path = resolve_path(argv[1]);
    int state = 0;
    if (!new_path){
        state = -1;
    }
    // 디렉토리 제거
    if (rmdir(new_path) != 0)
        state = -1;
    free(new_path);
    if (state == -1)
        write(write_fd, "e", 1);
    else
        write(write_fd, "s", 1);
    return state;
}

// 사용법 출력 함수
void usage_rmdir() {
    printf("Usage: rmdir [-p] [-v] directory ...\n");
    printf("  -p  Remove parent directories if they are empty\n");
    printf("  -v  Verbose mode: print each directory as it is removed\n");
}
