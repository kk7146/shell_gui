#include "libcmd.h"

// 파일 권한 출력 함수
static void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : (S_ISLNK(mode)) ? "l" : (S_ISFIFO(mode)) ? "p" :
           (S_ISCHR(mode)) ? "c" : (S_ISBLK(mode)) ? "b" : (S_ISSOCK(mode)) ? "s" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

// ln 명령어 함수
int cmd_ln(int argc, char **argv, int write_fd) {
    int opt;
    int s_flag = 0;  // -s 옵션 플래그
    int f_flag = 0;     // -f 옵션 플래그
    int v_flag = 0;   // -v 옵션 플래그
    int n_flag = 0;  // -n 옵션 플래그
    char *new_src;
    char *new_dest;

    // getopt를 사용하여 옵션 파싱
    while ((opt = getopt(argc, argv, "sfvn")) != -1) {
        switch (opt) {
            case 's':
                s_flag = 1;  // 심볼릭 링크 플래그 설정
                break;
            case 'f':
                f_flag = 1;  // 강제 덮어쓰기 플래그 설정
                break;
            case 'v':
                v_flag = 1;  // 자세한 출력 플래그 설정
                break;
            case 'n':
                n_flag = 1;  // 심볼릭 링크를 역참조하지 않음
                break;
            default:
                usage_ln();
        }
    }
    // 원본 및 링크 이름 처리
    if (optind >= argc - 1) {
        printf("ln: missing original or new link argument\n");
        usage_ln();
        return -2;
    }

    new_src = resolve_path(argv[optind]);
    if (check_null_pointer(new_src)) {
        return -1;
    }
    new_dest = resolve_path(argv[optind + 1]);
    if (check_null_pointer(new_dest)) {
        free(new_src);
        return -1;
    }

    // 강제 덮어쓰기 처리
    if (f_flag && access(new_dest, F_OK) == 0) { // 존재여부 확인
        if (unlink(new_dest) != 0) {
            perror("ln -f");
            free(new_src);
            free(new_dest);
            return -1;
        }
    }

    // 심볼릭 링크 생성
    if (s_flag) {
        if (symlink(new_src, new_dest) != 0) {
            perror("ln -s");
            free(new_src);
            free(new_dest);
            return -1;
        } else if (v_flag) {
            printf("Symbolic link created: %s -> %s\n", argv[optind + 1], argv[optind]);
        }
    } else {  // 하드 링크 생성
        if (link(new_src, new_dest) != 0) {
            perror("ln");
            free(new_src);
            free(new_dest);
            return -1;
        } else if (v_flag)
            printf("Hard link created: %s -> %s\n", argv[optind + 1], argv[optind]);
    }

    free(new_src);
    free(new_dest);
    return 0;
}

// ln 사용법 출력 함수
void usage_ln() {
    printf("Usage: ln [-s] [-f] [-v] [-n] <original> <new>\n");
    printf("  -s  Create a symbolic link instead of a hard link\n");
    printf("  -f  Force link creation by removing existing destination\n");
    printf("  -v  Verbosely describe link creation\n");
    printf("  -n  Do not dereference symbolic links (not fully implemented) (아직 미구현)\n");
}
