#include "libcmd.h"

static int apply_permissions(char *perm_str, const char *filename, int verbose, int f_flag, int h_flag, int R_flag, int v_flag, int H_flag, int L_flag, int P_flag) {
    struct stat statbuf;
    mode_t mode;

    if (stat(filename, &statbuf) != 0) {
        perror("stat");
        return -1;
    }
    mode = statbuf.st_mode;

    if (perm_str[0] >= '0' && perm_str[0] <= '7') {
        mode_t new_mode = strtol(perm_str, NULL, 8);
        if (chmod(filename, new_mode) != 0) {
            perror("chmod");
            return -1;
        } else if (verbose) {
            printf("Permissions changed to %o for %s\n", new_mode, filename);
        }
    } else {
        //내부 로직 아직 미구현.
        if (chmod(filename, mode) != 0) {
            return -1;
            perror("chmod");
        } else if (verbose) {
            printf("Permissions changed for %s\n", filename);
        }
    }
    return 0;
}

int cmd_chmod(int argc, char **argv, int write_fd) {
    int opt;
    int f_flag = 0, h_flag = 0, R_flag = 0, v_flag = 0;
    int H_flag = 0, L_flag = 0, P_flag = 1;
    char *perm_str = NULL;
    char *file_path;
    int state = 0;

    while ((opt = getopt(argc, argv, "fhvR:HLP")) != -1) {
        switch (opt) {
            case 'f':
                f_flag = 1;
                break;
            case 'h':
                h_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 'R':
                R_flag = 1;
                break;
            case 'H':
                H_flag = 1;
                L_flag = 0;
                P_flag = 0;
                break;
            case 'L':
                L_flag = 1;
                H_flag = 0;
                P_flag = 0;
                break;
            case 'P':
                P_flag = 1;
                H_flag = 0;
                L_flag = 0;
                break;
            default:
                usage_chmod();
        }
    }

    if (optind >= argc - 1) {
        usage_chmod();
        return -2;
    }

    perm_str = argv[optind++]; // 권한 문자열만 따로 구별.
    
    for (int i = optind; i < argc; i++) { // 여러 파일.
        file_path = resolve_path(argv[i]);
        if (file_path == NULL) {
            usage_chmod();
            state = -1;
            continue;
        }
        if (apply_permissions(perm_str, file_path, v_flag, f_flag, h_flag, R_flag , v_flag, H_flag, L_flag, P_flag) == -1)
            state = -1;
        free(file_path);
    }
    return state;
}

void usage_chmod() {
    printf("Usage: chmod [-fhv] [-R [-H | -L | -P]] mode file ... (세부 기능 아직 미구현.)\n");
}
