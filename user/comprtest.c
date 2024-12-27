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

// Test compression with detailed output
void test_compression(char *test_name, char *data, int len) {
    printf("\n=== Test Case: %s ===\n", test_name);
    printf("Original Data (%d bytes):\n", len);
    print_data_sample(data, len);
    
    // Create and write to test file
    int fd = open("test.txt", O_WRONLY | O_CREATE);
    if(fd < 0) {
        printf("Failed to create test file\n");
        exit(1);
    }
    
    // Write the original data
    if(write(fd, data, len) != len) {
        printf("Failed to write test file\n");
        close(fd);
        exit(1);
    }
    close(fd);
    
    // Get file stats before reading back
    struct stat st;
    if(stat("test.txt", &st) < 0) {
        printf("Failed to get file stats\n");
        exit(1);
    }
    
    printf("\nFile Statistics:\n");
    printf("Size on disk: %d bytes\n", (int)st.size);
    
    // Read back and verify
    fd = open("test.txt", O_RDONLY);
    if(fd < 0) {
        printf("Failed to open file for reading\n");
        exit(1);
    }
    
    char *read_buf = malloc(len + 1);
    if(!read_buf) {
        printf("Failed to allocate read buffer\n");
        close(fd);
        exit(1);
    }
    
    int bytes_read = read(fd, read_buf, len);
    close(fd);
    read_buf[bytes_read] = '\0';
    
    printf("\nVerification:\n");
    printf("Bytes read back: %d\n", bytes_read);
    printf("Read Data:\n");
    print_data_sample(read_buf, bytes_read);
    
    // Compare original and read data
    int match = (bytes_read == len && memcmp(data, read_buf, len) == 0);
    printf("\nVerification Result: %s\n", match ? "PASSED" : "FAILED");
    if (!match) {
        printf("Expected length: %d, Got: %d\n", len, bytes_read);
        if (bytes_read == len) {
            printf("Data content mismatch!\n");
        }
    }
    
    free(read_buf);
    unlink("test.txt");
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
    test_compression("File Compression Test", buf, size);
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
