#include "client.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#define MAX_RESPONSE_SIZE 1024
#define MAX_CMD_SIZE 128

int client(int read_fd, int write_fd) {
    char *command;
    char *response;

    command = (char*)malloc(MAX_CMD_SIZE);
    response = (char*)malloc(MAX_CMD_SIZE);
    while (1) {
        // 사용자 입력
        printf("Enter command: ");
        memset(command, 0, MAX_CMD_SIZE);
        fgets(command, MAX_CMD_SIZE, stdin);
        command[strcspn(command, "\n")] = '\0'; // 개행 문자 제거
        write(write_fd, command, strlen(command) + 1);
        memset(response, 0, MAX_CMD_SIZE);
        read(read_fd, response, MAX_RESPONSE_SIZE);
        if (strcmp(command, "quit") == 0)
            break;
        printf("--------------------\n");
        printf("Server : \n%s\n", response);
    }
    free(command);
    free(response);
    return 0;
}
