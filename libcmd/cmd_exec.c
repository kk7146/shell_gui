#include "libcmd.h"

int cmd_exec(int argc, char **argv, int write_fd) {
    int opt;
    int v_flag = 0;  // -v 옵션 플래그 (자세한 출력)
    int n_flag = 0; // -n 옵션 플래그 (명령어 출력만 하고 실행하지 않음)

    while ((opt = getopt(argc, argv, "vn")) != -1) {
        switch (opt) {
            case 'v':
                v_flag = 1;
                break;
            case 'n':
                n_flag = 1;
                break;
            default:
                usage_exec();
        }
    }

    // 명령어가 없는 경우
    if (optind >= argc) {
        usage_exec();
        return -2;
    }

    // 명령어를 하나의 문자열로 결합
    char command[MAX_CMD_SIZE] = {0};
    for (int i = optind; i < argc; i++) {
        strcat(command, argv[i]);
        if (i < argc - 1) {
            strcat(command, " ");
        }
    }

    // -v 옵션: 명령어를 출력
    if (v_flag) {
        printf("Executing command: %s\n", command);
    }

    // -n 옵션: 명령어를 출력만 하고 실행하지 않음
    if (n_flag) {
        printf("Command not executed due to -n option: %s\n", command);
        return 0;
    }

    int result = system(command);
    if (result == -1) {
        perror("system");
        return -1;
    } else {
        printf("Command executed with result: %d\n", result);
        return 0;
    }
}

void usage_exec() {
    printf("Usage: exec [-v] [-n] <command> [arguments...]\n");
    printf("  -v  Verbose mode: print the command before executing\n");
    printf("  -n  No-execute mode: print the command but do not execute\n");
}
