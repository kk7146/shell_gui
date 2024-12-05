#include "commandmanager.h"

void add_command(cmd_node **head, char *name, cmd_func_t cmd, usage_func_t usage) {
    cmd_node *new_node = (cmd_node *)malloc(sizeof(cmd_node));
    if (new_node == NULL) 
        return;

    strcpy(new_node->cmd_str, name);
    new_node->cmd_func = cmd;
    new_node->usage_func = usage;
    new_node->next = *head;
    *head = new_node;
}

// 명령어 제거 함수
void remove_command(cmd_node **head, char *name) {
    cmd_node *current = *head;
    cmd_node *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->cmd_str, name) == 0) {
            if (prev == NULL) {
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
}

// 명령어 찾기 함수
cmd_node* find_command(cmd_node *head, char *name) {
    cmd_node *current = head;

    while (current != NULL) {
        if (strcmp(current->cmd_str, name) == 0) {
            return current;  // 찾음
        }
        current = current->next;
    }
    return NULL;  // 찾지 못함
}

// 모든 명령어 해제 함수
void free_all_commands(cmd_node **head) {
    cmd_node *current = *head;
    while (current != NULL) {
        cmd_node *next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}

void help(cmd_node *head) { // 단순한 help와 quit는 기본 제공 cmd로 둘 것임.
    cmd_node *current = head;
    printf("Available commands:\n");
	printf("help                   : Show this help message\n");
	printf("quit                   : quit shell\n");
    while (current != NULL) {
        printf("  %s:\n", current->cmd_str);
        if (current->usage_func != NULL) {
            current->usage_func();
        } else {
            printf("  Usage information not available.\n");
        }
        current = current->next;
    }
}
