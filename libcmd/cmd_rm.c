#include "libcmd.h"

// 파일 또는 디렉토리를 삭제하는 함수
static int rm_func(const char *path, int f_flag, int i_flag, int r_flag, int v_flag) {
    struct stat statbuf;
    if (lstat(path, &statbuf) == -1) {
        if (!f_flag) {
            perror("rm");
        }
        return -1;
    }

    // 디렉토리 처리
    if (S_ISDIR(statbuf.st_mode)) {
        if (!r_flag) {
            if (!f_flag) {
                fprintf(stderr, "rm: %s: is a directory\n", path);
            }
            return -1;
        }

        DIR *dir = opendir(path);
        if (!dir) {
            if (!f_flag) {
                perror("rm");
            }
            return -1;
        }

        struct dirent *entry;
        char subpath[1024];
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            snprintf(subpath, sizeof(subpath), "%s/%s", path, entry->d_name);
            rm_func(subpath, f_flag, i_flag, r_flag, v_flag);
        }
        closedir(dir);

        // 디렉토리 삭제
        if (rmdir(path) == -1) {
            if (!f_flag) {
                perror("rm");
                return -1;
            }
        } else if (v_flag) {
            printf("removed directory: %s\n", path);
        }
        return 0;
    }

    // 파일 삭제 전 확인 요청
    if (i_flag) {
        printf("rm: remove file '%s'? (y/n): ", path);
        char response = getchar();
        if (response != 'y' && response != 'Y') {
            printf("rm: operation canceled for %s\n", path);
            return 0;
        }
    }

    // 파일 삭제
    if (unlink(path) == -1) {
        if (!f_flag) {
            perror("rm");
            return -1;
        }
    } else if (v_flag) {
        printf("removed: %s\n", path);
    }
    return 0;
}

// rm 명령어 함수
int cmd_rm(int argc, char **argv, int write_fd) {
    int opt;
    int f_flag = 0;       // -f 옵션 플래그 (강제 삭제)
    int i_flag = 0; // -i 옵션 플래그 (삭제 전 확인)
    int r_flag = 0;   // -R 또는 -r 옵션 플래그 (재귀 삭제)
    int v_flag = 0;     // -v 옵션 플래그 (자세한 출력)
    char *file_name;
    int state = 0;

    // getopt를 사용하여 옵션 파싱
    optind = 0;
    while ((opt = getopt(argc, argv, "fidRrv")) != -1) {
        switch (opt) {
            case 'f':
                f_flag = 1;
                i_flag = 0; // -f는 -i를 무시
                break;
            case 'i':
                i_flag = 1;
                f_flag = 0; // -i는 -f를 무시
                break;
            case 'd':
                break;
            case 'R':
            case 'r':
                r_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            default:
                usage_rm();
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "rm: missing operand\n");
        usage_rm();
        return -2;
    }

    for (int i = optind; i < argc; i++) {
        file_name = resolve_path(argv[i]);
        
        state = rm_func(argv[i], f_flag, i_flag, r_flag, v_flag);
        free(file_name);
    }
    if (state == 0)
        write(write_fd, "s", 1);
    else
        write(write_fd, "e", 1);
    return state;
}

// rm 사용법 출력 함수
void usage_rm() {
    printf("Usage: rm [-f | -i] [-dRrv] file ...\n");
    printf("  -f  Force removal of files without prompting\n");
    printf("  -i  Prompt before every removal\n");
    printf("  -d  Remove empty directories\n");
    printf("  -R, -r  Remove directories and their contents recursively\n");
    printf("  -v  Explain what is being done\n");
}
