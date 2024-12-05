#include "customshell.h"

static int ensure_directory_exists() { // BASE_DIR이 존재하는지 확인하고 없으면 만듦
    struct stat st;

    if (stat(BASE_DIR, &st) != 0)
        if (mkdir(BASE_DIR, 0755) != 0) {
            perror("mkdir");
            return -1;
        }
    return 0;
}

static void init_dir() { // 현재 위치가 BASE_DIR 안쪽이 아니면 BASE_DIR로 옮김
    char *current_dir;

    current_dir = getcwd(NULL, 0);
    if (!validate_path(current_dir))
        chdir(BASE_DIR);
}

static void handle_sigint(int sig) {
}

static void input_argc_argv(int *input_argc, char **input_argv, char *command) {
    char *tok_str;

    // 초기화
    *input_argc = 0;

    tok_str = strtok(command, " \n");
    while (tok_str != NULL && *input_argc < MAX_ARG - 1) {
        input_argv[(*input_argc)++] = tok_str;
        tok_str = strtok(NULL, " \n");
    }

    input_argv[*input_argc] = NULL;
}

int init() { // 프로그램 실행 경로가 BASE_DIR이 아닌 경우 해당 경로로 이동. 없으면 파일 만들고. 
    if (ensure_directory_exists() == -1)
        return -1;
    init_dir();
    return 0;
}

//ls cp rm rmdir chmod rename

int execute_shell(char *command, cmd_node *const head, int read_fd, int write_fd) {
    cmd_node *node;

    signal(SIGINT, handle_sigint);
    while (1) {
        char *input_argv[MAX_ARG];
        int  input_argc;
        char *tok_str;

        node = head;

        //if (fgets(command, MAX_CMD_SIZE-1, stdin) == NULL)
            //return -1;
        memset(command, 0, MAX_CMD_SIZE);
        read(read_fd, command, MAX_CMD_SIZE);
        if (command[0] == '\0')
            continue;
        else if (strcmp(command, "help") == 0) {
            help(node);
            write(write_fd, "s", 1);
        }
        else if (strcmp(command, "quit") == 0) {
            return 0;
        }
        else {
            input_argc_argv(&input_argc, input_argv, command);
            node = find_command(node, input_argv[0]);
            if (node != NULL)
            {
                optind = 0;// 1대신 0으로 초기화 해야 한다고 한다. 재초기화를 위해서
                node->cmd_func(input_argc, input_argv, write_fd);
            }
        }
    }
    return -1;
}


//while (1) {
//            // 부모로부터 명령 읽기
//            memset(command, 0, MAX_CMD_SIZE);
//            read(parent_to_child[0], command, MAX_CMD_SIZE);
//
//            // "quit" 명령 처리
//            if (strcmp(command, "quit") == 0) {
//                snprintf(response, MAX_RESPONSE_SIZE, "Server shutting down.\n");
//                write(child_to_parent[1], response, strlen(response) + 1);
//                break;
//            }
//
//            // 명령 처리
//            memset(response, 0, MAX_RESPONSE_SIZE);
//            process_command(command, response, head);
//
//            // 응답 전송
//            write(child_to_parent[1], response, strlen(response) + 1);
//        }