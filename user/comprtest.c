#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAX_FILE_SIZE 8192  // 8KB max file size for testing

// Print both ASCII and hex representation of data
void print_data_sample(char *data, int len) {
    printf("ASCII: \"");
    for(int i = 0; i < len && i < 50; i++) {
        if(data[i] >= 32 && data[i] <= 126) // printable ASCII
            printf("%c", data[i]);
        else
            printf(".");
    }
    printf("\"\n");

    printf("HEX:   ");
    for(int i = 0; i < len && i < 24; i++) {
        printf("%02x ", (unsigned char)data[i]);
    }
    if(len > 24) printf("...");
    printf("\n");
}

// Read entire file into buffer
int read_file(char *filename, char *buf, int max_size) {
    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        printf("Failed to open input file %s\n", filename);
        return -1;
    }

    struct stat st;
    if(stat(filename, &st) < 0) {
        printf("Failed to stat file %s\n", filename);
        close(fd);
        return -1;
    }

    if(st.size > max_size) {
        printf("File too large (max size: %d bytes)\n", max_size);
        close(fd);
        return -1;
    }

    int bytes_read = read(fd, buf, st.size);
    close(fd);

    if(bytes_read != st.size) {
        printf("Failed to read entire file\n");
        return -1;
    }

    return bytes_read;
}

// Print readable content and hex dump
void print_data(char *data, int len) {
    printf("\nContent (%d bytes):\n", len);
    write(1, data, len);
    printf("\n\nHex dump:\n");
    
    for(int i = 0; i < len; i++) {
        if(i % 16 == 0) {
            if(i > 0) {
                printf("  ");
                // Print ASCII for previous line
                for(int j = i-16; j < i; j++) {
                    char c = data[j];
                    printf("%c", (c >= 32 && c <= 126) ? c : '.');
                }
                printf("\n");
            }
            printf("%04x:", i);
        }
        printf(" %02x", (unsigned char)data[i]);
    }
    
    // Print padding spaces for last line
    int remaining = len % 16;
    if(remaining > 0) {
        for(int i = 0; i < (16-remaining)*3; i++)
            printf(" ");
    }
    
    // Print ASCII for last line
    printf("  ");
    int start = len - (len % 16);
    for(int i = start; i < len; i++) {
        char c = data[i];
        printf("%c", (c >= 32 && c <= 126) ? c : '.');
    }
    printf("\n\n");
}

// Test compression with detailed output
void test_compression(char *test_name, char *data, int len) {
    printf("\n=== Test Case: %s ===\n", test_name);
    
    // Create temporary test file
    char *filename = "temp.txt";
    int fd = open(filename, O_CREATE | O_WRONLY);
    if(fd < 0) {
        printf("Failed to create test file\n");
        return;
    }
    
    // Write test data
    if(write(fd, data, len) != len) {
        printf("Failed to write test data\n");
        close(fd);
        return;
    }
    close(fd);
    
    // Print original data
    printf("\nOriginal data:");
    print_data(data, len);
    
    // Get file stats
    struct stat st;
    if(stat(filename, &st) < 0) {
        printf("Failed to get file stats\n");
        unlink(filename);
        return;
    }
    
    printf("\nCompression results:\n");
    printf("Original size: %d bytes\n", len);
    printf("Size on disk: %d bytes\n", (int)st.size);
    
    // Read back and verify
    fd = open(filename, O_RDONLY);
    if(fd < 0) {
        printf("Failed to open file for reading\n");
        unlink(filename);
        return;
    }
    
    char buf[1024];
    int n = read(fd, buf, sizeof(buf));
    close(fd);
    
    if(n != len || memcmp(data, buf, len) != 0) {
        printf("Data verification failed!\n");
    } else {
        printf("Data verification passed\n");
    }
    
    unlink(filename);
}

void test_file(char *filename) {
    char *buf = malloc(MAX_FILE_SIZE);
    if(!buf) {
        printf("Failed to allocate buffer for file reading\n");
        exit(1);
    }

    int size = read_file(filename, buf, MAX_FILE_SIZE);
    if(size < 0) {
        free(buf);
        exit(1);
    }

    printf("\nTesting file: %s\n", filename);
    test_compression(filename, buf, size);
    free(buf);
}

int main(int argc, char *argv[]) {
    printf("Testing Huffman Compression Implementation\n");
    printf("----------------------------------------\n");

    if(argc > 1) {
        // Test input file
        test_file(argv[1]);
    } else {
        // Run standard test cases
        // Test case 1: Simple repeated pattern
        char test1[] = "AAAAABBBBBCCCCCDDDDD";
        test_compression("Simple Repeated Pattern", test1, strlen(test1));
        
        // Test case 2: Normal text
        char test2[] = "Hello, this is a test of compression.";
        test_compression("Normal Text", test2, strlen(test2));
        
        // Test case 3: Long repeating sequence
        char test3[128];
        for(int i = 0; i < 128; i++) {
            test3[i] = 'A' + (i % 4);  // Repeating ABCD pattern
        }
        test_compression("Long Repeating Sequence", test3, 128);
        
        // Test case 4: Binary pattern
        char test4[64];
        for(int i = 0; i < 64; i++) {
            test4[i] = i % 256;  // 0-255 repeating pattern
        }
        test_compression("Binary Pattern", test4, 64);
    }
    
    printf("\nAll compression tests completed.\n");
    return 0;
}
