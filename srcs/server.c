#include "server.h"
#include "customshell.h"

extern cmd_list list[];

static void command_set(int argc, char **argv, cmd_node** head)
{
    if (argc > 1) {
            for (int i = 1; i < argc; i++) {
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

int server(int read_fd, int write_fd, int argc, char** argv) {
    char *command;
    cmd_node *head = NULL;

    //초기화
    if (init() == -1) {
        return -1;
    }
    // argv를 통해 명령어 추가
    command_set(argc, argv, &head);
    command = (char*)malloc(MAX_CMD_SIZE);

    if (execute_shell(command, head, read_fd, write_fd) == -1) {
        free(command);
        return -1;
    }
    free(command);
    free_all_commands(&head);
    return 0;
}
