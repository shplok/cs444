#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "minheap.h"

// Name: Sawyer Bowerman

// Note: Original acknowledgment of GFG influence
// The geeks for geeks implementation of the huffman code problem was extremely informative and useful for helping me to figure out how to implement.

// Structure to store Huffman codes - Original implementation
struct HuffmanCode {
    unsigned char bits[256];  // The code itself
    int length;              // Length of the code
};

// Store codes for each character - Original implementation
struct HuffmanCode codes[256];

// Function to store codes during tree traversal
// [GFG Influence] Core logic similar to GFG's printCodes function, adapted to store codes instead of printing
void storeCodes(struct Node* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1);
    }

    // If leaf node, store the code - Original implementation for storage
    if (!root->left && !root->right) {
        for (int i = 0; i < top; i++) {
            codes[root->inputs].bits[i] = arr[i];
        }
        codes[root->inputs].length = top;
    }
}

// [GFG Influence] Print huffman codes - Very similar to GFG's printCodes function
void printCodes(struct Node* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    if (!root->left && !root->right) {
        printf("%c: ", root->inputs);
        for (int i = 0; i < top; i++)
            printf("%d", arr[i]);
        printf("\n");
    }
}

// [GFG Influence] Build the huffman tree structure - Core algorithm similar to GFG's buildHuffmanTree
struct Node* buildHuffmanTree(int freq[]) {
    struct MinHeap* minHeap = createMinHeap(256);

    // Create initial nodes for chars with freq > 0
    for (int i = 0; i < 256; i++) {
        if (freq[i] != 0) {
            insertMinHeap(minHeap, newNode(i, freq[i]));
        }
    }

    // [GFG Influence] Combine 2 minimum nodes repetitively - Similar to GFG's tree building logic
    while (minHeap->size > 1) {
        struct Node* left = extractMin(minHeap);
        struct Node* right = extractMin(minHeap);

        struct Node* top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    return extractMin(minHeap);
}

// Original implementation - Memory management
void freeTree(struct Node* root) {
    if (root == NULL) return;
    
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// Original implementation - Bit manipulation and file writing
void writeBits(FILE* outFp, unsigned char* buffer, int* bufferLen, unsigned char bit) {
    *buffer = (*buffer << 1) | bit;
    (*bufferLen)++;
    
    if (*bufferLen == 8) {
        fwrite(buffer, 1, 1, outFp);
        *buffer = 0;
        *bufferLen = 0;
    }
}

// Original implementation - Main function with file handling and command line parsing
int main(int argc, char *argv[]) {
    char *inFile = "completeShakespeare.txt";
    char *outFile = "huffman.out";
    int debug = 0;
    int opt;
    
    while ((opt = getopt(argc, argv, "i:o:d")) != -1) {
        switch (opt) {
            case 'i':
                inFile = optarg;
                break;
            case 'o':
                outFile = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-i input] [-o output] [-d]\n", argv[0]);
                exit(1);
        }
    }

    // Original implementation - File handling and frequency counting
    FILE *fp = fopen(inFile, "r");
    if (!fp) {
        perror("Error opening input file");
        return 1;
    }

    int freq[256] = {0};
    int c;
    int totalChars = 0;
    
    while ((c = fgetc(fp)) != EOF) {
        freq[c]++;
        totalChars++;
    }

    if (debug) {
        printf("Character frequencies:\n");
        for (int i = 0; i < 256; i++) {
            if (freq[i] > 0) {
                printf("'%c': %d\n", i, freq[i]);
            }
        }
        printf("Total characters: %d\n", totalChars);
    }

    // [GFG Influence] Building and using the Huffman tree
    struct Node* root = buildHuffmanTree(freq);

    int arr[256] = {0}, top = 0;
    storeCodes(root, arr, top);

    if (debug) {
        printf("\nHuffman Codes:\n");
        printCodes(root, arr, top);
    }

    // Original implementation - File encoding and output
    FILE *outFp = fopen(outFile, "wb");
    if (!outFp) {
        perror("Error opening output file");
        fclose(fp);
        freeTree(root);
        return 1;
    }

    fseek(fp, 0, SEEK_SET);

    unsigned char buffer = 0;
    int bufferLen = 0;

    while ((c = fgetc(fp)) != EOF) {
        for (int i = 0; i < codes[c].length; i++) {
            writeBits(outFp, &buffer, &bufferLen, codes[c].bits[i]);
        }
    }

    if (bufferLen > 0) {
        buffer <<= (8 - bufferLen);
        fwrite(&buffer, 1, 1, outFp);
    }

    fclose(fp);
    fclose(outFp);
    freeTree(root);

    return 0;
}