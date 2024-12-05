#include "util.h"

int validate_path(char *path) { // 유효한 위치인지 확인.
    char absolute_path[MAX_CMD_SIZE];
    realpath(path, absolute_path);
    if (strncmp(BASE_DIR, absolute_path, strlen(BASE_DIR)) != 0)
        return 0;
    return 1;
}

char* resolve_path(const char *path) { // path를 받고 유효한 위치로 변환해서 리턴.
    char *resolved_path = (char*)malloc(MAX_CMD_SIZE);  // 메모리 할당
    if (resolved_path == NULL) {
        perror("malloc failed");
        return NULL;
    }
    printf("1\n");
    if (path[0] == '/')
        snprintf(resolved_path, MAX_CMD_SIZE, "%s%s", BASE_DIR, path);
    else
    {
        char *cwd = getcwd(NULL, 0);  // 현재 작업 디렉토리 가져오기
        if (cwd == NULL) {
            perror("getcwd failed");
            free(resolved_path);
            return NULL;
        }
        // snprintf에 올바른 형식 지정자 사용
        snprintf(resolved_path, MAX_DIR_SIZE, "%s/%s", cwd, path);
        free(cwd);  // getcwd에서 할당된 메모리 해제
    }
    printf("1.%s\n", resolved_path);
    if (!validate_path(resolved_path))
    {
        free(resolved_path);
        return NULL;
    }
    printf("2.%s\n", resolved_path);
    return resolved_path;
}

int check_null_pointer(const char *ch) { // malloc 터질 우려
    if (ch == NULL) {
        perror("malloc");
        return 1;
    }
    return 0;
}
