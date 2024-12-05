#include "libcmd.h"

static int cp_func(const char *source, const char *destination) {
    FILE *src_file = fopen(source, "r");
    if (src_file == NULL) {
        perror("cp (source)");
        return -1;
    }
    FILE *dest_file = fopen(destination, "w");
    if (dest_file == NULL) {
        perror("cp (destination)");
        fclose(src_file);
        return -1;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes, dest_file);
    }
    fclose(src_file);
    fclose(dest_file);
    return 0;
}

int cmd_cp(int argc, char **argv, int write_fd) {
    int opt;
    char *src = NULL;
    char *dest = NULL;

    src = resolve_path(argv[1]);
    printf("%s\n", src);
    if (check_null_pointer(src)) {
        write(write_fd, "e", 1);
        return -1;
    }
    dest = resolve_path(argv[2]);
    if (check_null_pointer(dest)) {
        free(src);
        write(write_fd, "e", 1);
        return -1;
    }

    if (cp_func(src, dest) == -1)
    {
        free(src);
        free(dest);
        write(write_fd, "e", 1);
        return -1;
    }
    write(write_fd, "s", 1);
    free(src);
    free(dest);
    return 0;
}

void usage_cp() {
    printf("Usage: cp [-R [-H | -L | -P]] [-fi | -n] [-alpSsvXx] <source> <dest> (옵션 미구현 v만 구현)\n");
}
