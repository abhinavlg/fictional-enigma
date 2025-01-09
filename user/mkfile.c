#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: mkfile <filename> <content>\n");
        exit(1);
    }

    int fd = open(argv[1], O_CREATE | O_WRONLY);
    if(fd < 0) {
        printf("Failed to create file %s\n", argv[1]);
        exit(1);
    }

    if(write(fd, argv[2], strlen(argv[2])) != strlen(argv[2])) {
        printf("Failed to write to file\n");
        close(fd);
        exit(1);
    }

    close(fd);
    exit(0);
} 