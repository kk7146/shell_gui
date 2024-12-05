#include "libcmd.h"
#include <ctype.h>

static void cat_func(FILE *file, int b_flag, int e_flag, int n_flag, int s_flag, int t_flag, int u_flag, int v_flag) { // 플래그에 따른 구현부
    char line[1024];
    int line_number = 1;
    int previous_blank = 0;

    while (fgets(line, sizeof(line), file) != NULL) {

        if (s_flag)
        {
            if (line[0] == '\n') {
                if (previous_blank)
                    continue;
                previous_blank = 1;
            }
            else
                previous_blank = 0;
        }

        // 줄 수 보이게 처리. b의 경우에는 아무 것도 없는 줄의 경우만 출력.
        if (n_flag || (b_flag && line[0] != '\n'))
            printf("%6d  ", line_number++);

        //이제 전처리 끝. 문자 출력
        if (e_flag || t_flag || v_flag) {
            for (char *p = line; *p != '\0'; p++) {
                if (iscntrl((unsigned char)*p) && *p != '\n' && *p != '\t') {
                    if ((unsigned char)*p == 127)
                        printf("^?");
                    else
                        printf("^%c", *p + '@');
                }
                else if ((unsigned char)*p >= 128)
                    printf("M-%c", (unsigned char)*p & 0x7F);
                else if (*p == '\n') {
                    if (e_flag)
                        printf("$\n");
                    else
                        putchar(*p);
                }
                // 탭 문자 처리 (-t 옵션)
                else if (*p == '\t') {
                    if (t_flag)
                        printf("^I");
                    else
                        putchar(*p);
                }
                else
                    putchar(*p);
            }
        }
        else
            printf("%s", line);
    }
}

int cmd_cat(int argc, char **argv, int write_fd) { // cat 명령어 구현부
    char *file_path;
    int opt;
    int b_flag = 0, e_flag = 0, n_flag = 0, s_flag = 0, t_flag = 0, u_flag = 0, v_flag = 0;
    int state = 0;

    while ((opt = getopt(argc, argv, "benstuv")) != -1) {
        switch (opt) {
            case 'b':
                b_flag = 1;
                n_flag = 0;  // -b의 경우에 -n은 삭제
                break;
            case 'e':
                t_flag = 0;
                v_flag = 0;
                e_flag = 1;
                break;
            case 'n':
                b_flag = 0;
                n_flag = 1;  // -n의 경우에 -b은 삭제
                break;
            case 's':
                s_flag = 1;
                break;
            case 't':
                t_flag = 1;
                v_flag = 0;
                e_flag = 0;
                break;
            case 'u':
                u_flag = 1;
                break;
            case 'v':
                t_flag = 0;
                v_flag = 1;
                e_flag = 0;
                break;
            default:
                usage_cat();
        }
    }

    if (optind >= argc) {
        usage_cat();
        return -2;
    }

    for (int i = optind; i < argc; i++) { // optind로 변환 후 끝까지 파일 다 읽어봄.
        file_path = resolve_path(argv[i]);
        if (file_path == NULL)
        {
            printf("cat: invalid filename argument\n");
            state = -1;
            continue;
        }
        FILE *file = fopen(file_path, "r");
        if (file == NULL) {
            perror("cat");
            free(file_path);
            state = -1;
            continue;
        }
        cat_func(file, b_flag, e_flag, n_flag, s_flag, t_flag, u_flag, v_flag);
        free(file_path);
        fclose(file);
    }
    return state;
}

void usage_cat() {
    printf("Usage: cat [-belnstuv] [file ...]\n");
    printf("  -b  Number the non-blank output lines, starting at 1\n");
    printf("  -e  Display non-printing characters and end lines with $\n");
    printf("  -n  Number all output lines, starting at 1\n");
    printf("  -s  Squeeze multiple adjacent empty lines\n");
    printf("  -t  Display non-printing characters and show tabs as ^I\n");
    printf("  -u  Disable output buffering (미구현)\n");
    printf("  -v  Display non-printing characters. \n      Control characters print as \'^X\' for control-X; the delete character (octal 0177) prints as \'^?\'. \n      Non-ASCII characters (with the high bit set) are printed as \'M-\' (for meta) followed by the character for the low 7 bits.\n");
}
