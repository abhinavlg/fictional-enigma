#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"

#define MAX_TREE_NODES 512  // Maximum nodes in Huffman tree
#define MAX_HEAP_SIZE 256   // Maximum size of priority queue
#define BITS_PER_BYTE 8

// Global variables
static struct huffman_node tree[MAX_TREE_NODES];
static int heap[MAX_HEAP_SIZE];
static int heap_size = 0;
static int tree_size = 0;

// Priority queue operations
static void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void min_heapify(int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap_size && tree[heap[left]].freq < tree[heap[smallest]].freq)
        smallest = left;
    if (right < heap_size && tree[heap[right]].freq < tree[heap[smallest]].freq)
        smallest = right;

    if (smallest != i) {
        swap(&heap[i], &heap[smallest]);
        min_heapify(smallest);
    }
}

void insert_heap(int node_idx) {
    if (heap_size >= MAX_HEAP_SIZE)
        return;

    heap[heap_size] = node_idx;
    int i = heap_size++;
    
    while (i > 0 && tree[heap[(i - 1) / 2]].freq > tree[heap[i]].freq) {
        swap(&heap[i], &heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int extract_min() {
    if (heap_size <= 0)
        return -1;

    int min = heap[0];
    heap[0] = heap[--heap_size];
    min_heapify(0);
    return min;
}

// Build Huffman tree
void build_tree(char *input, int inlen) {
    int freq[256] = {0};
    tree_size = 0;
    heap_size = 0;

    // Count frequencies
    for (int i = 0; i < inlen; i++)
        freq[(unsigned char)input[i]]++;

    // Create leaf nodes
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            tree[tree_size].c = i;
            tree[tree_size].freq = freq[i];
            tree[tree_size].left = -1;
            tree[tree_size].right = -1;
            insert_heap(tree_size++);
        }
    }

    // Build tree
    while (heap_size > 1) {
        int left = extract_min();
        int right = extract_min();

        tree[tree_size].freq = tree[left].freq + tree[right].freq;
        tree[tree_size].left = left;
        tree[tree_size].right = right;
        insert_heap(tree_size++);
    }
}

// Write bit to output buffer
void write_bit(char *output, int *byte_pos, int *bit_pos, int bit) {
    if (*bit_pos == 0)
        output[*byte_pos] = 0;

    output[*byte_pos] |= (bit << (7 - *bit_pos));
    *bit_pos = (*bit_pos + 1) % 8;
    if (*bit_pos == 0)
        (*byte_pos)++;
}

// Compress using Huffman coding
int compress_huffman(char *input, int inlen, char *output, int maxlen) {
    // Add debug prints
    printf("Building Huffman tree for %d bytes...\n", inlen);
    
    if (!input || !output || inlen <= 0 || maxlen <= 0)
        return -1;

    // Build Huffman tree
    build_tree(input, inlen);
    
    // Write tree size and tree to output
    if (sizeof(int) + sizeof(struct huffman_node) * tree_size >= maxlen)
        return -1;
    
    // Write tree size first
    *(int*)output = tree_size;
    memmove(output + sizeof(int), tree, sizeof(struct huffman_node) * tree_size);
    
    // Compress data
    int byte_pos = sizeof(int) + sizeof(struct huffman_node) * tree_size;
    int bit_pos = 0;
    
    for (int i = 0; i < inlen; i++) {
        unsigned char c = input[i];
        int curr_node = tree_size - 1;  // Start from root
        
        // Find character in tree and write path
        while (tree[curr_node].left != -1 || tree[curr_node].right != -1) {
            if (byte_pos >= maxlen)
                return -1;
                
            if (c < tree[tree[curr_node].left].c) {
                write_bit(output, &byte_pos, &bit_pos, 0);
                curr_node = tree[curr_node].left;
            } else {
                write_bit(output, &byte_pos, &bit_pos, 1);
                curr_node = tree[curr_node].right;
            }
        }
    }
    
    // Pad last byte if necessary
    if (bit_pos > 0)
        byte_pos++;
        
    // Add size checks
    if(byte_pos >= inlen) {
        printf("Compression ineffective (got %d bytes)\n", byte_pos);
        return -1;
    }
    
    printf("Compressed to %d bytes\n", byte_pos);
    return byte_pos;
}

// Read bit from input buffer
int read_bit(char *input, int *byte_pos, int *bit_pos) {
    int bit = (input[*byte_pos] >> (7 - *bit_pos)) & 1;
    *bit_pos = (*bit_pos + 1) % 8;
    if (*bit_pos == 0)
        (*byte_pos)++;
    return bit;
}

// Decompress using Huffman coding
int decompress_huffman(char *input, int inlen, char *output, int maxlen) {
    if (!input || !output || inlen <= 0 || maxlen <= 0)
        return -1;

    // Read tree size from input
    tree_size = *(int*)input;
    if (tree_size <= 0 || tree_size > MAX_TREE_NODES)
        return -1;
        
    // Read tree from input
    if (sizeof(int) + sizeof(struct huffman_node) * tree_size > inlen)
        return -1;
        
    memmove(tree, input + sizeof(int), sizeof(struct huffman_node) * tree_size);
    
    // Decompress data
    int in_pos = sizeof(int) + sizeof(struct huffman_node) * tree_size;
    int out_pos = 0;
    int bit_pos = 0;
    
    while (in_pos < inlen && out_pos < maxlen) {
        int curr_node = tree_size - 1;  // Start from root
        
        // Traverse tree until leaf
        while (tree[curr_node].left != -1 || tree[curr_node].right != -1) {
            if (in_pos >= inlen)
                break;
                
            int bit = read_bit(input, &in_pos, &bit_pos);
            curr_node = bit ? tree[curr_node].right : tree[curr_node].left;
        }
        
        if (tree[curr_node].left == -1 && tree[curr_node].right == -1)
            output[out_pos++] = tree[curr_node].c;
    }
    
    return out_pos;
}
