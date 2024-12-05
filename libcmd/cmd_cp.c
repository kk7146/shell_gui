#include "libcmd.h"

static int cp_func(const char *source, const char *destination, int verbose) {
    FILE *src_file = fopen(source, "r");
    if (src_file == NULL) {
        perror("cp (source)");
        return -1;
    }
    FILE *dest_file = fopen(destination, "w");
    if (dest_file == NULL) {
        perror("cp (destination)");
        fclose(src_file);
        return -1;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes, dest_file);
    }
    fclose(src_file);
    fclose(dest_file);
    if (verbose) {
        printf("File copied from %s to %s\n", source, destination);
    }
    return 0;
}

int cmd_cp(int argc, char **argv, int write_fd) {
    int opt;
    char *src = NULL;
    char *dest = NULL;
    int R_flag = 0, H_flag = 0, L_flag = 0, P_flag = 1;  // 재귀 및 심볼릭 링크 관련 옵션
    int f_flag = 0, i_flag = 0, n_flag = 0, a_flag = 0;  // 복사 제어 옵션
    int l_flag = 0, p_flag = 0, S_flag = 0, s_flag = 0;  // 속성 관련 옵션
    int v_flag = 0, X_flag = 0, x_flag = 0;              // 기타 옵션

    while ((opt = getopt(argc, argv, "RHLPfinaSlpSsvXx")) != -1) {
        switch (opt) {
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
            case 'f':
                f_flag = 1;
                break;
            case 'i':
                i_flag = 1;
                break;
            case 'n':
                n_flag = 1;
                break;
            case 'a':
                a_flag = 1;
                break;
            case 'l':
                l_flag = 1;
                break;
            case 'p':
                p_flag = 1;
                break;
            case 'S':
                S_flag = 1;
                break;
            case 's':
                s_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 'X':
                X_flag = 1;
                break;
            case 'x':
                x_flag = 1;
                break;
            default:
                usage_cp();
        }
    }

    if (optind >= argc - 1) {
        fprintf(stderr, "cp: missing source or destination argument\n");
        usage_cp();
        return -2;
    }

    src = resolve_path(argv[optind++]);
    if (check_null_pointer(src)) {
        return -1;
    }
    dest = resolve_path(argv[optind]);
    if (check_null_pointer(dest)) {
        free(src);
        return -1;
    }

    if (cp_func(src, dest, v_flag) == -1)
    {
        free(src);
        free(dest);
        return -1;
    }
    free(src);
    free(dest);
    return 0;
}

void usage_cp() {
    printf("Usage: cp [-R [-H | -L | -P]] [-fi | -n] [-alpSsvXx] <source> <dest> (옵션 미구현 v만 구현)\n");
}
