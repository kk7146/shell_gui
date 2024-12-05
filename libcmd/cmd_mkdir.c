#include "libcmd.h"

static int mkdir_func(int argc, char **argv, int p_flag, int v_flag, int mode, int i) {
    char *dir_name = resolve_path(argv[i]);

    
    // -p 옵션이 있는 경우, 부모 디렉토리도 생성
    if (p_flag) {
        // 경로를 하나씩 생성
        char temp_path[MAX_DIR_SIZE];
        char *p = NULL;
        snprintf(temp_path, sizeof(temp_path), "%s", dir_name);

        for (p = temp_path + 1; *p; p++) {
            if (*p == '/') {
                *p = '\0'; // 슬래시를 임시로 종료 문자로 바꿈
                if (mkdir(temp_path, mode) == -1 && errno != EEXIST) {
                    perror("mkdir");
                    return -1;
                }
                *p = '/'; // 슬래시 복원
            }
        }
        // 마지막 디렉토리 생성
        if (mkdir(temp_path, mode) == -1 && errno != EEXIST) {
            perror("mkdir");
            return -1;
        }
    }
    else // -p 옵션이 없는 경우, 디렉토리 생성
        if (mkdir(dir_name, mode) == -1) { // 파일을 만들지 못하는 경우(이미 있는 경우도 오류)
            perror("mkdir");
            return -1;
        }
    // -v 옵션이 있는 경우, 생성된 디렉토리 이름 출력
    if (v_flag)
        printf("mkdir: created directory '%s'\n", argv[i]);
    return 0;
}


int cmd_mkdir(int argc, char **argv, int write_fd) {
    int opt;
    int p_flag = 0;  // -p 옵션 플래그
    int v_flag = 0;  // -v 옵션 플래그
    mode_t mode = 0777;  // 권한
    int state = 0;

    optind = 0;
    while ((opt = getopt(argc, argv, "pvm:")) != -1) {
        switch (opt) {
            case 'p':
                p_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 'm':
                mode = strtol(optarg, NULL, 8);  // 8진수
                break;
            default:
                usage_mkdir();
        }
    }

    // optind getopt()가 처리하지 않은 첫 번째 인덱스
    for (int i = optind; i < argc; i++) {
        if (mkdir_func(argc, argv, p_flag, v_flag, mode, i) == -1)
            state = -1;
    }
    if (state == -1)
        write(write_fd, "e", 2);
    else 
        write(write_fd, "s", 2);
    return state;
}

void usage_mkdir() {
    printf("Usage: mkdir [-pv] [-m mode] directory_name ...\n");
}
