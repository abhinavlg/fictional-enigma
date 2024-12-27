#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void create_and_test(char *filename, char *content) {
    printf("\n=== Testing with file: %s ===\n", filename);
    
    // Create file
    int fd = open(filename, O_CREATE | O_WRONLY);
    if(fd < 0) {
        printf("Failed to create %s\n", filename);
        exit(1);
    }
    
    // Write content
    if(write(fd, content, strlen(content)) != strlen(content)) {
        printf("Failed to write to %s\n", filename);
        close(fd);
        exit(1);
    }
    close(fd);
    
    // Show original content
    printf("\nOriginal file contents:\n");
    fd = open(filename, O_RDONLY);
    char buf[1024];
    int n = read(fd, buf, sizeof(buf));
    close(fd);
    write(1, buf, n);
    printf("\n");
    
    // Test compression
    printf("\nTesting compression:\n");
    char *args[] = {"comprtest", filename, 0};
    exec("comprtest", args);
}

int main() {
    printf("Starting compression tests\n");
    printf("-------------------------\n");

    // Test 1: Repeated pattern
    create_and_test("test1.txt", 
        "AAAAABBBBBCCCCCDDDDDAAAAABBBBBCCCCCDDDDD");
    
    // Test 2: Normal text
    create_and_test("test2.txt",
        "This is a normal text file with some content that might be compressible.\n"
        "Let's see how well the Huffman compression handles this kind of data.\n");
    
    // Test 3: Highly compressible
    char test3[512];
    for(int i = 0; i < 511; i++)
        test3[i] = 'A' + (i % 3);
    test3[511] = '\0';
    create_and_test("test3.txt", test3);

    printf("\nAll compression tests completed.\n");
    exit(0);
} 