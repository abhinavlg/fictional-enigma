#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"

#define LZ77_MAX_WINDOW 256
#define LZ77_MAX_LENGTH 15
#define MAX_COMPRESSED_SIZE 4096

// Helper function to get minimum of two numbers
static inline int min(int a, int b) {
    return a < b ? a : b;
}

// Simplified LZ77 compression
int compress_lz77(char *input, int inlen, char *output, int maxlen) {
    if (!input || !output || inlen <= 0 || maxlen <= 0)
        return -1;

    int in_pos = 0;
    int out_pos = 0;

    // Leave room for at least 4 bytes at the end for final literals
    while (in_pos < inlen && out_pos < maxlen - 4) {
        int best_dist = 0;
        int best_len = 0;

        // Only try matching if we have enough remaining bytes
        if (inlen - in_pos > 4) {
            // Search back in window
            int window_start = (in_pos > LZ77_MAX_WINDOW) ? 
                (in_pos - LZ77_MAX_WINDOW) : 0;

            for (int j = window_start; j < in_pos; j++) {
                int match_len = 0;
                int max_possible = min(LZ77_MAX_LENGTH, inlen - in_pos - 1);

                // Find match length
                while (match_len < max_possible &&
                       input[j + match_len] == input[in_pos + match_len]) {
                    match_len++;
                }

                // Update best match if better than current
                if (match_len > best_len) {
                    best_len = match_len;
                    best_dist = in_pos - j;
                }
            }
        }

        // Output token or literal
        if (best_len > 2 && out_pos + 3 < maxlen - 4) {
            output[out_pos++] = best_dist & 0xFF;
            output[out_pos++] = best_len & 0xFF;
            output[out_pos++] = input[in_pos + best_len];
            in_pos += best_len + 1;
        } else {
            // Use literal for small matches or near end of input
            output[out_pos++] = input[in_pos++];
        }
    }

    // Copy any remaining bytes as literals
    while (in_pos < inlen && out_pos < maxlen) {
        output[out_pos++] = input[in_pos++];
    }

    return (in_pos >= inlen) ? out_pos : -1;
}

// Decompress LZ77 data
int decompress_lz77(char *input, int inlen, char *output, int maxlen) {
    if (!input || !output || inlen <= 0 || maxlen <= 0)
        return -1;

    int in_pos = 0;
    int out_pos = 0;

    while (in_pos < inlen && out_pos < maxlen) {
        // Check if we have enough bytes for a potential token (3 bytes minimum)
        if (inlen - in_pos >= 3) {
            uint dist = (uint)(input[in_pos] & 0xFF);
            uint len = (uint)(input[in_pos + 1] & 0xFF);
            
            // Validate token - strict bounds checking
            if (dist > 0 && dist <= out_pos && 
                len > 0 && len <= LZ77_MAX_LENGTH && 
                out_pos + len + 1 <= maxlen &&  // +1 for the next char
                in_pos + 2 < inlen) {  // Ensure we can read the next char
                
                char next = input[in_pos + 2];
                in_pos += 3;
                
                // Copy the referenced sequence
                int start = out_pos - dist;
                if (start >= 0) {
                    // Carefully copy matched sequence
                    for (uint i = 0; i < len && out_pos < maxlen; i++) {
                        if (start + i >= out_pos) break;  // Prevent invalid references
                        output[out_pos++] = output[start + i];
                    }
                    
                    // Add the next character
                    if (out_pos < maxlen) {
                        output[out_pos++] = next;
                    }
                } else {
                    // Invalid back reference, treat as literal
                    if (out_pos < maxlen) {
                        output[out_pos++] = input[in_pos - 3];
                        in_pos -= 2;  // Rewind to next byte
                    }
                }
            } else {
                // Not a valid token, treat as literal
                if (out_pos < maxlen) {
                    output[out_pos++] = input[in_pos++];
                }
            }
        } else {
            // Not enough bytes left for a token, copy remaining as literals
            while (in_pos < inlen && out_pos < maxlen) {
                output[out_pos++] = input[in_pos++];
            }
        }
    }

    return out_pos;
}
