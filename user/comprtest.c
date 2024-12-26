#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"

void print_data_sample(char *data, int len) {
    write(1, "Data sample: \"", 14);
    if (len > 0) {
        write(1, data, len < 20 ? len : 20);
    }
    printf("...\" (showing first 20 of %d bytes)\n", len);
}

int main(int argc, char *argv[])
{
  if(argc < 2) {
    printf("Usage: comprtest <pattern>\n");
    printf("Example: comprtest aaaaaabbbbb\n");
    exit(1);
  }
  
  int pattern_len = strlen(argv[1]);
  char *test_data = argv[1];  // Use input directly
  
  printf("\nInput pattern length: %d\n", pattern_len);
  write(1, "Input pattern: \"", 16);
  write(1, test_data, pattern_len);
  printf("\"\n");
  
  printf("\n=== Writing Test Data ===\n");
  print_data_sample(test_data, pattern_len);
  
  // Create and write to file
  int fd = open("comptest.txt", O_WRONLY | O_CREATE);
  if(fd < 0) {
    printf("Failed to create file\n");
    exit(1);
  }
  
  // Write the actual pattern
  int bytes_written = write(fd, test_data, pattern_len);
  printf("Bytes written: %d\n", bytes_written);
  
  close(fd);
  
  // Get file stats
  struct stat st;
  if(stat("comptest.txt", &st) < 0) {
    printf("Failed to get file stats\n");
    exit(1);
  }
  
  printf("\n=== Compression Results ===\n");
  printf("Original size: %d bytes\n", pattern_len);
  printf("Size on disk: %d bytes\n", (int)st.size);
  if (st.size < pattern_len) {
    int saved = ((pattern_len - st.size) * 100) / pattern_len;
    printf("Space saved: %d%%\n", saved);
  }
  
  printf("\n=== Verification ===\n");
  fd = open("comptest.txt", O_RDONLY);
  if(fd < 0) {
    printf("Failed to open file for reading\n");
    exit(1);
  }
  
  char *read_buf = malloc(pattern_len + 1);
  if(!read_buf) {
    printf("Failed to allocate read buffer\n");
    exit(1);
  }
  
  int bytes_read = read(fd, read_buf, pattern_len);
  read_buf[bytes_read] = '\0';
  
  printf("Bytes read back: %d\n", bytes_read);
  print_data_sample(read_buf, bytes_read);
  printf("Data verification: %s\n", 
         memcmp(test_data, read_buf, bytes_read) == 0 ? "PASSED" : "FAILED");
  
  close(fd);
  free(read_buf);
  exit(0);
}
