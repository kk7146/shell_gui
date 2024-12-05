#include "libcmd.h"
#include <libgen.h>

int cmd_rmdir(int argc, char **argv, int write_fd) {
    int opt;
    int p_flag = 0;    // -p 옵션 플래그 (부모 디렉토리 제거)
    int v_flag = 0;   // -v 옵션 플래그 (자세한 출력)
    int state = 0;

    while ((opt = getopt(argc, argv, "pv")) != -1) {
        switch (opt) {
            case 'p':
                p_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            default:
                usage_rmdir();
        }
    }

    // 옵션 처리 후 남은 인자는 제거할 디렉토리 경로들
    if (optind >= argc) {
        printf("rmdir: missing operand\n");
        usage_rmdir();
        return -2;
    }

    // 각 디렉토리 경로에 대해 처리
    for (int i = optind; i < argc; i++) {
        char *new_path = resolve_path(argv[i]);
        if (!new_path) {
            printf("rmdir: failed to resolve path for %s\n", argv[i]);
            state = -1;
            continue;
        }

        // 디렉토리 제거
        if (rmdir(new_path) != 0) {
            state = -1;
            perror("rmdir");
        } else if (v_flag) {
            printf("removed directory: %s\n", argv[i]);
        }

        free(new_path);
    }
    return state;
}

// 사용법 출력 함수
void usage_rmdir() {
    printf("Usage: rmdir [-p] [-v] directory ...\n");
    printf("  -p  Remove parent directories if they are empty\n");
    printf("  -v  Verbose mode: print each directory as it is removed\n");
}
