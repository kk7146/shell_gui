#include "libcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int signal_name_to_number(const char *signal_name) {
    if (strcmp(signal_name, "HUP") == 0) return SIGHUP;
    if (strcmp(signal_name, "INT") == 0) return SIGINT;
    if (strcmp(signal_name, "QUIT") == 0) return SIGQUIT;
    if (strcmp(signal_name, "ABRT") == 0) return SIGABRT;
    if (strcmp(signal_name, "KILL") == 0) return SIGKILL;
    if (strcmp(signal_name, "ALRM") == 0) return SIGALRM;
    if (strcmp(signal_name, "TERM") == 0) return SIGTERM;
    return -1;
}

// 모든 시그널 이름 나열
void list_signals() {
    printf(" 1: HUP\n");
    printf(" 2: INT\n");
    printf(" 3: QUIT\n");
    printf(" 6: ABRT\n");
    printf(" 9: KILL\n");
    printf("14: ALRM\n");
    printf("15: TERM\n");
}

int cmd_kill(int argc, char **argv, int write_fd) {
    int opt;
    int signal = SIGTERM;
    int list_flag = 0;
    char *signal_name = NULL;
    int state = 0;

    while ((opt = getopt(argc, argv, "s:l")) != -1) {
        switch (opt) {
            case 's':
                signal_name = optarg;
                break;
            case 'l':
                list_flag = 1;
                break;
            default:
                usage_kill();
        }
    }

    // 시그널 목록 출력
    if (list_flag) {
        list_signals();
        return 0;
    }

    // 시그널 이름을 시그널 번호로 변환
    if (signal_name != NULL) {
        signal = signal_name_to_number(signal_name);
        if (signal == -1) {
            printf("Invalid signal name: %s\n", signal_name);
            return -2;
        }
    }

    // PID 처리
    if (optind >= argc) {
        printf("Missing PID\n");
        usage_kill();
        return -2;
    }

    for (int i = optind; i < argc; i++) {
        pid_t pid = atoi(argv[i]);
        if (pid <= 0) {
            printf("Invalid PID: %s\n", argv[i]);
            state = -2;
            continue;
        }

        // 시그널 전송
        if (kill(pid, signal) == 0) {
            printf("Signal %d sent to process %d.\n", signal, pid);
        } else {
            perror("kill");
            state = -1;
        }
    }
    return state;
}

void usage_kill() {
    printf("Usage: kill [-s signal_name] <pid> ...\n");
    printf("  -s signal_name  : Specify the signal to be sent (default is TERM)\n");
    printf("  -l              : List all signal names\n");
}
