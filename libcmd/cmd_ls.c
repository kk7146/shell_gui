#include "libcmd.h"

static int ls_func(int a_flag, int l_flag, int s_flag, int r_flag, int write_fd) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    struct passwd *pw;
    struct group *gr;
    char timebuf[64];
    char link_target[MAX_DIR_SIZE + 1];
    char buffer[8192]; // 결과를 모아둘 메인 출력 버퍼
    char temp_buffer[1024]; // 각 파일의 출력을 위한 임시 버퍼
    size_t buffer_offset = 0; // 메인 출력 버퍼의 현재 위치
    int state = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("ls");
        return -2;
    }

    // 디렉토리 엔트리 순회
    while ((entry = readdir(dir)) != NULL) {
        if (!a_flag && entry->d_name[0] == '.') {
            continue;  // -a 옵션이 없는 경우 숨김 파일 무시
        }
        if (lstat(entry->d_name, &statbuf) == -1) {
            perror("lstat");
            state = -1;
            continue;
        }

        // 상세 정보 생성 (-l 옵션)
        if (l_flag) {
            snprintf(temp_buffer, sizeof(temp_buffer), "%c%c%c%c%c%c%c%c%c%c %ld %s %s %5lld %s %s",
                (S_ISDIR(statbuf.st_mode)) ? 'd' :
                (S_ISLNK(statbuf.st_mode)) ? 'l' :
                (S_ISFIFO(statbuf.st_mode)) ? 'p' :
                (S_ISCHR(statbuf.st_mode)) ? 'c' :
                (S_ISBLK(statbuf.st_mode)) ? 'b' :
                (S_ISSOCK(statbuf.st_mode)) ? 's' : '-',
                (statbuf.st_mode & S_IRUSR) ? 'r' : '-',
                (statbuf.st_mode & S_IWUSR) ? 'w' : '-',
                (statbuf.st_mode & S_IXUSR) ? 'x' : '-',
                (statbuf.st_mode & S_IRGRP) ? 'r' : '-',
                (statbuf.st_mode & S_IWGRP) ? 'w' : '-',
                (statbuf.st_mode & S_IXGRP) ? 'x' : '-',
                (statbuf.st_mode & S_IROTH) ? 'r' : '-',
                (statbuf.st_mode & S_IWOTH) ? 'w' : '-',
                (statbuf.st_mode & S_IXOTH) ? 'x' : '-',
                (long)statbuf.st_nlink,
                (pw = getpwuid(statbuf.st_uid)) ? pw->pw_name : "?",
                (gr = getgrgid(statbuf.st_gid)) ? gr->gr_name : "?",
                (long long)statbuf.st_size,
                strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&statbuf.st_mtime)) ? timebuf : "?",
                entry->d_name
            );
        } else
            snprintf(temp_buffer, sizeof(temp_buffer), "%s", entry->d_name);

        // 심볼릭 링크 대상 처리
        if (S_ISLNK(statbuf.st_mode)) {
            ssize_t len = readlink(entry->d_name, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                strncat(temp_buffer, " -> ", sizeof(temp_buffer) - strlen(temp_buffer) - 1);
                strncat(temp_buffer, link_target, sizeof(temp_buffer) - strlen(temp_buffer) - 1);
            }
        }

        strncat(temp_buffer, "\n", sizeof(temp_buffer) - strlen(temp_buffer) - 1);

        // 메인 버퍼에 추가
        size_t temp_len = strlen(temp_buffer);
        if (buffer_offset + temp_len < sizeof(buffer)) {
            memcpy(buffer + buffer_offset, temp_buffer, temp_len);
            buffer_offset += temp_len;
        } else {
            fprintf(stderr, "Buffer overflow, increase buffer size.\n");
            state = -1;
            break;
        }
    }

    // 모든 내용을 한 번에 write
    if (buffer_offset > 0) {
        if (write(write_fd, buffer, buffer_offset) == -1) {
            perror("write");
            state = -1;
        }
    }
    else
        write(write_fd, "e", 1);

    closedir(dir);
    return state;
}

// ls 명령어 함수
int cmd_ls(int argc, char **argv, int write_fd) {
    int opt;
    int a_flag = 0;     // -a 옵션 플래그 (숨김 파일 표시)
    int l_flag = 0;  // -l 옵션 플래그 (상세 정보 표시)
    int s_flag = 0;    // -t 옵션 플래그 (시간 기준 정렬)
    int r_flag = 0;      // -r 옵션 플래그 (역순 정렬)

    // getopt를 사용하여 옵션 파싱
    argv[argc] = NULL;
    optind = 1;
    while ((opt = getopt(argc, argv, "altr")) != -1) {
        switch (opt) {
            case 'a':
                a_flag = 1;
                break;
            case 'l':
                l_flag = 1;
                break;
            case 't':
                s_flag = 1;
                break;
            case 'r':
                r_flag = 1;
                break;
            default:
                usage_ls();
        }
    }
    return ls_func(a_flag, l_flag, s_flag, r_flag, write_fd);
}

// ls 사용법 출력 함수
void usage_ls() {
    printf("Usage: ls [-a] [-l] [-t] [-r]\n");
    printf("  -a  List all entries including hidden files\n");
    printf("  -l  Use a long listing format\n");
    printf("  -t  Sort by modification time, newest first (미구현)\n");
    printf("  -r  Reverse the order of the sort (미구현)\n");
}
