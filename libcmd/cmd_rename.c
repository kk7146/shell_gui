#include "libcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// rename 명령어 함수
int cmd_rename(int argc, char **argv, int write_fd) {
    int opt;
    int f_flag = 0;   // -f 옵션 플래그 (강제 덮어쓰기)
    int i_flag = 0; // -i 옵션 플래그 (대화형 모드)
    int v_flag = 0; // -v 옵션 플래그 (자세한 출력)
    char *new_src;
    char *new_dest;

    while ((opt = getopt(argc, argv, "fiv")) != -1) {
        switch (opt) {
            case 'f':
                f_flag = 1;
                break;
            case 'i':
                i_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            default:
                usage_rename();
        }
    }

    // 남은 인자에서 source와 target 가져오기
    if (optind >= argc - 1) {
        printf("rename: missing source or target argument\n");
        usage_rename();
        return -2;
    }

    new_src = resolve_path(argv[optind]);
    new_dest = resolve_path(argv[optind + 1]);

    // malloc 실패 처리
    if (check_null_pointer(new_src) || check_null_pointer(new_dest)) {
        if (new_src) free(new_src);
        if (new_dest) free(new_dest);
        printf("rename: failed to allocate memory\n");
        return -1;
    }

    // 대화형 모드: 기존 파일이 있으면 사용자에게 확인 요청
    if (i_flag && access(new_dest, F_OK) == 0) {
        printf("rename: overwrite '%s'? (y/n): ", argv[optind + 1]);
        char response = getchar();
        if (response != 'y' && response != 'Y') {
            printf("rename: operation canceled\n");
            free(new_src);
            free(new_dest);
            return 0;
        }
    }

    // rename 호출
    if (rename(new_src, new_dest) != 0) {
        perror("rename");
        free(new_src);
        free(new_dest);
        return -1;
    }
    else if (v_flag) {
        printf("Renamed '%s' to '%s'\n", argv[optind], argv[optind + 1]);
    }
    free(new_src);
    free(new_dest);
    return 0;
}

// rename 사용법 출력 함수
void usage_rename() {
    printf("Usage: rename [-f] [-i] [-v] <source> <target>\n");
    printf("  -f  Force overwrite if the target file exists (미구현)\n");
    printf("  -i  Interactive mode: ask before overwriting\n");
    printf("  -v  Verbose mode: display rename operation details\n");
}
