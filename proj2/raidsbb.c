#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
Sawyer Bowerman CS444

This project file consists of the RAID encryption method
using Hamming(7, 4).
*/

#define BLOCK_SIZE 512  // in bytes
#define DATA_DISKS 4
#define PARITY_DISKS 3
#define TOTAL_DISKS (DATA_DISKS + PARITY_DISKS)



// Lookup tablefor Hamming(7,4) code 
// This came as verbal advice from prior student, Hussein Watfa, Im not sure how giving credit for that should work.
static const uint8_t hamming_encode_table[16] = {
    0b0000000, // 0000 -> 0000000
    0b0001011, // 0001 -> 0001011
    0b0010111, // 0010 -> 0010111
    0b0011100, // 0011 -> 0011100
    0b0100110, // 0100 -> 0100110
    0b0101101, // 0101 -> 0101101
    0b0110001, // 0110 -> 0110001
    0b0111010, // 0111 -> 0111010
    0b1000101, // 1000 -> 1000101
    0b1001110, // 1001 -> 1001110
    0b1010010, // 1010 -> 1010010
    0b1011001, // 1011 -> 1011001
    0b1100011, // 1100 -> 1100011
    0b1101000, // 1101 -> 1101000
    0b1110100, // 1110 -> 1110100
    0b1111111  // 1111 -> 1111111
};


void create_files (const char *baseFilename, char *partFiles[TOTAL_DISKS]) {
    for (int i = 0; i < TOTAL_DISKS; i++) {
        partFiles[i] = malloc(strlen(baseFilename) + 10);
        sprintf(partFiles[i], "%s.part%d", baseFilename, i);
    }
}


void add_to_buffer(FILE *file, uint8_t *buffer, int *bitCount, uint8_t bit) {
    *buffer = (*buffer << 1) | (bit & 0x01); // use least signifcant bit 
    (*bitCount)++;
    
    if (*bitCount == 8) {
        fputc(*buffer, file);
        printf("Writing byte: %02X\n", *buffer);  // Debug output
        *buffer = 0;
        *bitCount = 0;
    }
}


void flush_buffers(FILE *files[TOTAL_DISKS], uint8_t buffers[TOTAL_DISKS], int bitCounts[TOTAL_DISKS]) {
    for (int i = 0; i < TOTAL_DISKS; i++) {
        if (bitCounts[i] > 0) {
            buffers[i] = buffers[i] << (8 - bitCounts[i]);
            fputc(buffers[i], files[i]);
        }
    }
}


int main(int argc, char *argv[]) {
    // similar to previous proj
    char *inFile = "completeShakespeare.txt";
    int debug = 0;
    int opt;
    int totalBytes = 0;
    
    while ((opt = getopt(argc, argv, "f:d")) != -1) { 
        switch (opt) {
            case 'f':
                inFile = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-f file input] [-d]\n", argv[0]);
                exit(1);
        }
    }

    FILE *input = fopen(inFile, "rb");
    if (!input) {
        perror("Error with opening input file");
        exit(1);
    }

    char *partFiles[TOTAL_DISKS];
    create_files(inFile, partFiles);
    FILE *outputs[TOTAL_DISKS];

    for (int i = 0; i < TOTAL_DISKS; i++) {
        outputs[i] = fopen(partFiles[i], "wb");
        if (!outputs[i]) {
            perror("Error opening output file");
            exit(1);
        }
        if (debug) {
            printf("Created output file: %s\n", partFiles[i]);
        }
    }

    uint8_t buffers[TOTAL_DISKS] = {0};
    int bitCounts[TOTAL_DISKS] = {0};
    int byte;

    while ((byte = fgetc(input)) != EOF) {
        totalBytes++;
        // Process high nibble (first 4 bits)
        uint8_t high_nibble = (byte >> 4) & 0x0F; // logical and on last 4 digits
        uint8_t high_hamming = hamming_encode_table[high_nibble];
        
        // for (int i = 0; i < TOTAL_DISKS; i++) {
        //     uint8_t bit = (high_hamming >> (TOTAL_DISKS - 1 - i)) & 0x01;
        //     add_to_buffer(outputs[i], &buffers[i], &bitCounts[i], bit);
        // }
        
        // first bit part0
        add_to_buffer(outputs[0], &buffers[0], &bitCounts[0], 
            (((high_hamming >> 6) & 0x01) + ((high_hamming >> 5) & 0x01) + ((high_hamming >> 3) & 0x01)) % 2 == 0 ? 0 : 1);
        add_to_buffer(outputs[1], &buffers[1], &bitCounts[1], (high_hamming >> 0) & 0x01);  // Part1 = bit 0
        add_to_buffer(outputs[2], &buffers[2], &bitCounts[2], (high_hamming >> 6) & 0x01);  // Part2 = bit 6
        add_to_buffer(outputs[3], &buffers[3], &bitCounts[3], (high_hamming >> 1) & 0x01);  // Keep Part3 = bit 1 (CORRECT)
        add_to_buffer(outputs[4], &buffers[4], &bitCounts[4], (high_hamming >> 5) & 0x01);  // Keep Part4 = bit 5 (CORRECT)
        add_to_buffer(outputs[5], &buffers[5], &bitCounts[5], (high_hamming >> 4) & 0x01);  // Part5 = bit 4
        add_to_buffer(outputs[6], &buffers[6], &bitCounts[6], (high_hamming >> 3) & 0x01);  // Keep Part6 = bit 3 (CORRECT)

        // Process low nibble (last 4 bits)
        uint8_t low_nibble = byte & 0x0F; 
        uint8_t low_hamming = hamming_encode_table[low_nibble];

        // for (int i = 0; i < TOTAL_DISKS; i++) {
        //     uint8_t bit = (low_hamming >> (TOTAL_DISKS - 1 - i)) & 0x01;
        //     add_to_buffer(outputs[i], &buffers[i], &bitCounts[i], bit);
        // }

        add_to_buffer(outputs[0], &buffers[0], &bitCounts[0], 
            (((low_hamming >> 6) & 0x01) + ((low_hamming >> 5) & 0x01) + ((low_hamming >> 3) & 0x01)) % 2 == 0 ? 0 : 1);
        add_to_buffer(outputs[1], &buffers[1], &bitCounts[1], (low_hamming >> 0) & 0x01);  // Part1 = bit 0
        add_to_buffer(outputs[2], &buffers[2], &bitCounts[2], (low_hamming >> 6) & 0x01);  // Part2 = bit 6
        add_to_buffer(outputs[3], &buffers[3], &bitCounts[3], (low_hamming >> 1) & 0x01);  // Keep Part3 = bit 1 (CORRECT)
        add_to_buffer(outputs[4], &buffers[4], &bitCounts[4], (low_hamming >> 5) & 0x01);  // Keep Part4 = bit 5 (CORRECT)
        add_to_buffer(outputs[5], &buffers[5], &bitCounts[5], (low_hamming >> 4) & 0x01);  // Part5 = bit 4
        add_to_buffer(outputs[6], &buffers[6], &bitCounts[6], (low_hamming >> 3) & 0x01);  // Keep Part6 = bit 3 (CORRECT)

    }

    
    if (debug) {
        printf("Input file: %s\n", inFile);
        printf("Output files: ");
        for (int i = 0; i < TOTAL_DISKS; i++) {
            printf("%s\n", partFiles[i]);
        
        }
    }

    flush_buffers(outputs, buffers, bitCounts);

    fclose(input);
    for (int i = 0; i < TOTAL_DISKS; i++) {
        fclose(outputs[i]);
        free(partFiles[i]);
        
    }

    return 0;
}