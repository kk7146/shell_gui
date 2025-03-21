#include "server.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int parent_to_child[2]; // 부모 -> 자식 파이프
    int child_to_parent[2]; // 자식 -> 부모 파이프
    pid_t pid;

    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("pipe");
        return -1;
    }
    pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    if (pid == 0) { // 자식 프로세스: 명령어 처리 서버
        close(parent_to_child[1]); // 부모 -> 자식 쓰기 닫기
        close(child_to_parent[0]); // 자식 -> 부모 읽기 닫기

        server(parent_to_child[0], child_to_parent[1]);

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    } else { // 부모 프로세스: 클라이언트 역할
        close(parent_to_child[0]); // 부모 -> 자식 읽기 닫기
        close(child_to_parent[1]); // 자식 -> 부모 쓰기 닫기

        client(child_to_parent[0], parent_to_child[1]);

        close(parent_to_child[1]);
        close(child_to_parent[0]);
        wait(NULL);
    }
}
