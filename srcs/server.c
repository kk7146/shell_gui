#include "server.h"
#include "customshell.h"

extern cmd_list list[];

static void command_set(int argc, char **argv, cmd_node** head)
{
    if (argc > 1) {
            for (int i = 0; i < argc; i++) {
                int is_found = 0;
                for (int j = 0; j < sizeof(list) / sizeof(list[0]); j++) {
                    if (strcmp(argv[i], list[j].name) == 0) {
                        add_command(head, list[j].name, list[j].cmd_func, list[j].usage_func);
                        is_found = 1;
                        break;
                    }
                }
                if (!is_found) {
                    printf("Warning: '%s' is not a recognized command and will be ignored.\n", argv[i]);
                }
            }
        }
}

void read_commands_from_file(const char *filename, int *argc, char **argv) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_CMD_SIZE];
    *argc = 0;

    // 파일에서 한 줄씩 읽고 argv에 저장
    while (fgets(line, sizeof(line), file) != NULL) {
        char *token = strtok(line, " \n");  // 공백과 개행으로 분리
        while (token != NULL) {
            argv[*argc] = strdup(token);  // 동적으로 메모리 할당 후 token 복사
            (*argc)++;
            token = strtok(NULL, " \n");
        }
    }

    fclose(file);
}

int server(int read_fd, int write_fd) {
    char *command;
    cmd_node *head = NULL;
    int argc = 0;
    char *argv[MAX_ARG];

    read_commands_from_file("./config", &argc, argv);
    command_set(argc, argv, &head);
    //초기화
    if (init() == -1) {
        return -1;
    }
    // argv를 통해 명령어 추가
    command = (char*)malloc(MAX_CMD_SIZE);

    if (execute_shell(command, head, read_fd, write_fd) == -1) {
        free(command);
        return -1;
    }
    free(command);
    free_all_commands(&head);
    return 0;
}
