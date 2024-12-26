#include "types.h"
#include "riscv.h"
#include "defs.h"

// RLE compression
int compress_rle(char *input, int inlen, char *output, int maxlen) {
    if (!input || !output || inlen <= 0 || maxlen <= 0)
        return -1;  // Invalid parameters

    int outidx = 0;
    int i = 0;

    while (i < inlen) {
        // Count repeated characters
        char curr = input[i];
        int count = 1;
        
        while (i + count < inlen && 
               input[i + count] == curr && 
               count < 255) {
            count++;
        }

        // Check if we have enough space in output buffer
        if (outidx + 2 > maxlen)
            return -1;  // Output buffer overflow

        // Write count and character
        output[outidx++] = count;
        output[outidx++] = curr;
        
        i += count;
    }

    return outidx;  // Return compressed size
}

// RLE decompression
int decompress_rle(char *input, int inlen, char *output, int maxlen) {
    if (!input || !output || inlen <= 0 || maxlen <= 0)
        return -1;  // Invalid parameters

    if (inlen % 2 != 0)
        return -1;  // Invalid compressed data (must be pairs)

    int outidx = 0;
    int i = 0;

    while (i < inlen) {
        int count = (unsigned char)input[i++];
        char value = input[i++];

        // Validate count
        if (count <= 0)
            return -1;  // Invalid count

        // Check for output buffer overflow
        if (outidx + count > maxlen)
            return -1;

        // Expand the run
        for (int j = 0; j < count; j++) {
            output[outidx++] = value;
        }
    }

    return outidx;  // Return decompressed size
}
