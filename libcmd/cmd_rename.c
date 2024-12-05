#include "libcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// rename 명령어 함수
int cmd_rename(int argc, char **argv, int write_fd) {
    char *new_src;
    char *new_dest;

    new_src = resolve_path(argv[1]);
    new_dest = resolve_path(argv[2]);

    // malloc 실패 처리
    if (check_null_pointer(new_src) || check_null_pointer(new_dest)) {
        if (new_src) free(new_src);
        if (new_dest) free(new_dest);
        write(write_fd, "e", 1);
        return -1;
    }

    // rename 호출
    if (rename(new_src, new_dest) != 0) {
        free(new_src);
        free(new_dest);
        write(write_fd, "e", 1);
        return -1;
    }
    free(new_src);
    free(new_dest);
    write(write_fd, "s", 1);
    return 0;
}

// rename 사용법 출력 함수
void usage_rename() {
    printf("Usage: rename [-f] [-i] [-v] <source> <target>\n");
    printf("  -f  Force overwrite if the target file exists (미구현)\n");
    printf("  -i  Interactive mode: ask before overwriting\n");
    printf("  -v  Verbose mode: display rename operation details\n");
}
