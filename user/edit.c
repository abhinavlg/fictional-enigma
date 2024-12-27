#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAXLINE 1024
#define KEY_DEL 127
#define KEY_RETURN '\r'
#define KEY_NEWLINE '\n'

char buf[MAXLINE];
int len = 0;

void save_file(char *filename) {
    int fd = open(filename, O_CREATE | O_WRONLY);
    if(fd < 0) {
        printf("Failed to create file %s\n", filename);
        return;
    }
    write(fd, buf, len);
    close(fd);
    printf("File saved: %s (%d bytes)\n", filename, len);
}

void load_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        printf("New file: %s\n", filename);
        return;
    }
    len = read(fd, buf, MAXLINE-1);
    if(len < 0) len = 0;
    buf[len] = '\0';
    close(fd);
}

void print_help() {
    printf("\nCommands:\n");
    printf("$ to save\n");
    printf("@ to exit\n");
}

void handle_input(char c) {
    // Don't process command characters as text
    if(c == '$' || c == '@') {
        return;
    }
    
    if(c == KEY_RETURN || c == KEY_NEWLINE) {
        if(len < MAXLINE-1) {
            buf[len++] = '\n';
            buf[len] = '\0';
            // Don't echo newline - terminal handles it
        }
    } else {
        if(c == KEY_DEL || c == '\b') {
            if(len > 0) {
                len--;
                buf[len] = '\0';
                // Terminal handles backspace display
            }
        }
        else if(len < MAXLINE-1 && c >= 32 && c < 127) {
            buf[len++] = c;
            buf[len] = '\0';
            // Don't echo character - terminal handles it
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: edit <filename>\n");
        exit(1);
    }

    char *filename = argv[1];
    load_file(filename);
    
    printf("Simple text editor. Type your text:\n");
    print_help();
    printf("Current content:\n");
    if(len > 0) {
        write(1, buf, len);
        if(buf[len-1] != '\n')
            write(1, "\n", 1);
    }
    
    char c;
    while(1) {
        if(read(0, &c, 1) <= 0) 
            break;

        if(c == '$') {  // Save
            save_file(filename);
            continue;
        }
        if(c == '@') {  // Exit
            printf("\nExit? (y/n): ");
            char confirm;
            while(read(0, &confirm, 1) > 0) {
                if(confirm == 'y' || confirm == 'Y') {
                    exit(0);
                }
                if(confirm == 'n' || confirm == 'N') {
                    printf("\n");
                    break;
                }
            }
            continue;
        }
        
        handle_input(c);
    }
    
    exit(0);
} 