#include "client.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PATH 1024
#define MAX_RESPONSE_SIZE 8192
#define MAX_CMD_SIZE 128

// 파일 정보를 저장할 구조체
typedef struct {
    char permissions[11];     // 파일 권한
    int links;                // 링크 수
    char owner[32];           // 소유자
    char group[32];           // 그룹
    long long size;           // 파일 크기
    char month[4];            // 수정 월 (예: Dec)
    char day[3];              // 수정 일 (예: 06)
    char time_or_year[6];     // 수정 시간 또는 연도 (예: 00:24 또는 2024)
    char name[256];           // 파일 이름
} FileInfo;

void debug_log(const char *format, ...) {
    FILE *log_file = fopen("debug.log", "a"); // 디버깅 로그 파일 열기
    if (!log_file) return;

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args); // 로그 파일에 출력
    va_end(args);

    fclose(log_file); // 파일 닫기
}

void draw_menu(WINDOW *win, FileInfo *files, int file_count, int highlight) {
    for (int i = 0; i < file_count; i++) {
        // 강조된 항목 처리
        if (i == highlight)
            wattron(win, A_REVERSE);
        // 파일 이름
        mvwprintw(win, i + 1, 1, "%s", files[i].name);
        if (i == highlight)
            wattroff(win, A_REVERSE);
    }
    box(win, 0, 0);
    wrefresh(win);
}

void draw_info(WINDOW *win, FileInfo *files, int file_count, int highlight) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, " File Info ");
    mvwprintw(win, 1, 1, "Name: %s", files[highlight].name);
    mvwprintw(win, 2, 1, "Permissions: %s", files[highlight].permissions);
    mvwprintw(win, 3, 1, "Links: %d", files[highlight].links);
    mvwprintw(win, 4, 1, "Owner: %s", files[highlight].owner);
    mvwprintw(win, 5, 1, "Group: %s", files[highlight].group);
    mvwprintw(win, 6, 1, "Size: %lld bytes", files[highlight].size);
    mvwprintw(win, 7, 1, "Modified: %s %s %s",
              files[highlight].month,
              files[highlight].day,
              files[highlight].time_or_year);
    wrefresh(win);
}

void parse_ls_output(const char *response, FileInfo **files, int *count) {
    int line_count = 0;
    size_t capacity = 16;  // 초기 크기
    *files = malloc(capacity * sizeof(FileInfo));
    if (!*files) {
        perror("malloc failed");
        *count = 0;
        return;
    }

    // 줄 단위로 파싱
    const char *line_start = response;
    while (*line_start) {
        const char *line_end = strchr(line_start, '\n');
        if (!line_end) line_end = line_start + strlen(line_start);

        // 줄 길이 계산
        size_t line_length = line_end - line_start;
        char line[1024];
        strncpy(line, line_start, line_length);
        line[line_length] = '\0';

        // 필요하면 공간 확장
        if (line_count >= capacity) {
            capacity *= 2;
            FileInfo *new_files = realloc(*files, capacity * sizeof(FileInfo));
            if (!new_files) {
                perror("realloc failed");
                free(*files);
                *files = NULL;
                *count = 0;
                return;
            }
            *files = new_files;
        }

        // 구조체에 정보 저장
        FileInfo *file = &(*files)[line_count];
        sscanf(line, "%10s %d %31s %31s %lld %3s %2s %5s %255[^\n]",
                file->permissions,    // 파일 권한
                &file->links,         // 링크 수
                file->owner,          // 소유자
                file->group,          // 그룹
                &file->size,          // 파일 크기
                file->month,          // 수정 월
                file->day,            // 수정 일
                file->time_or_year,   // 수정 시간 또는 연도
                file->name);          // 파일 이름

        line_count++;
        line_start = (*line_end) ? line_end + 1 : line_end;
    }

    *count = line_count;
}

void free_file_list(FileInfo *files) {
    if (files)
        free(files);
}

int client(int read_fd, int write_fd) {
    char response[MAX_RESPONSE_SIZE];
    int file_count = 0, highlight = 0;
    int ch;
    FileInfo *files = NULL;

    // ncurses 초기화
    initscr();
    clear();
    noecho();
    cbreak();

    // 창 크기 설정
    int menu_width = COLS / 2;  // 왼쪽 창 너비 (화면의 절반)
    int info_width = COLS - menu_width; // 오른쪽 창 너비
    int height = LINES - 4;     // 상단 두 창의 높이 (아래 단축키 설명 공간 제외)

    // 메뉴 창 생성
    WINDOW *menu_win = newwin(height, menu_width, 1, 0);
    box(menu_win, 0, 0);
    mvwprintw(menu_win, 0, 1, " File List ");
    wrefresh(menu_win);

    // 정보 창 생성
    WINDOW *info_win = newwin(height, info_width, 1, menu_width);
    box(info_win, 0, 0);
    mvwprintw(info_win, 0, 1, " File Info ");
    wrefresh(info_win);

    // 도움말 창 생성
    WINDOW *help_win = newwin(3, COLS, LINES - 3, 0); // 높이 3, 화면 하단 고정
    box(help_win, 0, 0);
    mvwprintw(help_win, 0, 1, " Help ");
    mvwprintw(help_win, 1, 1, "UP/DOWN: Navigate   ENTER: Select   Q: Quit");
    wrefresh(help_win);

    keypad(menu_win, TRUE);

    while (1) {
        // 자식 프로세스에 "ls" 명령 전송
        write(write_fd, "ls -al", 6);

        // 자식 프로세스의 응답 읽기
        memset(response, 0, sizeof(response));
        read(read_fd, response, sizeof(response));

        // 응답을 파일 목록으로 파싱
        parse_ls_output(response, &files, &file_count);
        wclear(menu_win);
        draw_menu(menu_win, files, file_count, highlight);
        draw_info(info_win, files, file_count, highlight);
        ch = wgetch(menu_win);

        switch (ch) {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < file_count - 1) highlight++;
                break;
            case 'q': // 종료
                write(write_fd, "quit", 5);
                free_file_list(files);
                delwin(menu_win);
                delwin(info_win);
                delwin(help_win);
                endwin();
                return 0;
            case 10: // Enter 키
            {
                if (files[highlight].permissions[0]=='d') {
                    char command[MAX_PATH];
                    snprintf(command, sizeof(command), "cd %s", files[highlight].name);
                    write(write_fd, command, strlen(command) + 1);
                    memset(response, 0, MAX_CMD_SIZE);
                    read(read_fd, response, MAX_RESPONSE_SIZE);
                    free_file_list(files);
                    highlight = 0;
                }
            }
                break;
            default:
                break;
        }
    }
    free_file_list(files);
    delwin(menu_win);
    delwin(info_win);
    delwin(help_win);
    endwin();
    return 0;
}