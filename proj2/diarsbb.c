#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


/*
Sawyer Bowerman CS444

This project file consists of the DIAR decryption and correction
 method, stemming from RAID2.
*/

#define DATA_DISKS 4
#define PARITY_DISKS 3
#define TOTAL_DISKS (DATA_DISKS + PARITY_DISKS)


// ------------------------------------------------------------------------------------ //


// Info about hamming decode values and constants from 
// https://github.com/MichaelDipperstein/hamming/blob/master/hamming.
// Copied and Modified

#define DATA_VALUES (1 << DATA_DISKS)

/* number of parity bits and data values */
#define PARITY_VALUES   (1 << PARITY_DISKS)

/* number of code bits (data + parity) and data values */
#define CODE_BITS       (DATA_DISKS + PARITY_DISKS)
#define CODE_VALUES     (1 << CODE_BITS)

// For error correction, maps parity_result to error position (0 means no error)
static const uint8_t error_pos[8] = {
    0, // 000 -> No error
    1, // 001 -> Error in bit 1
    2, // 010 -> Error in bit 2
    3, // 011 -> Error in bit 3
    4, // 100 -> Error in bit 4
    5, // 101 -> Error in bit 5
    6, // 110 -> Error in bit 6
    7  // 111 -> Error in bit 7
};

// lookup table for decoding hamming codes to data values
const unsigned char hamming_decode_values[CODE_VALUES] =
{
    0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0E, 0x07,     /* 0x00 to 0x07 */
    0x00, 0x09, 0x0E, 0x0B, 0x0E, 0x0D, 0x0E, 0x0E,     /* 0x08 to 0x0F */
    0x00, 0x03, 0x03, 0x03, 0x04, 0x0D, 0x06, 0x03,     /* 0x10 to 0x17 */
    0x08, 0x0D, 0x0A, 0x03, 0x0D, 0x0D, 0x0E, 0x0D,     /* 0x18 to 0x1F */
    0x00, 0x05, 0x02, 0x0B, 0x05, 0x05, 0x06, 0x05,     /* 0x20 to 0x27 */
    0x08, 0x0B, 0x0B, 0x0B, 0x0C, 0x05, 0x0E, 0x0B,     /* 0x28 to 0x2F */
    0x08, 0x01, 0x06, 0x03, 0x06, 0x05, 0x06, 0x06,     /* 0x30 to 0x37 */
    0x08, 0x08, 0x08, 0x0B, 0x08, 0x0D, 0x06, 0x0F,     /* 0x38 to 0x3F */
    0x00, 0x09, 0x02, 0x07, 0x04, 0x07, 0x07, 0x07,     /* 0x40 to 0x47 */
    0x09, 0x09, 0x0A, 0x09, 0x0C, 0x09, 0x0E, 0x07,     /* 0x48 to 0x4F */
    0x04, 0x01, 0x0A, 0x03, 0x04, 0x04, 0x04, 0x07,     /* 0x50 to 0x57 */
    0x0A, 0x09, 0x0A, 0x0A, 0x04, 0x0D, 0x0A, 0x0F,     /* 0x58 to 0x5F */
    0x02, 0x01, 0x02, 0x02, 0x0C, 0x05, 0x02, 0x07,     /* 0x60 to 0x67 */
    0x0C, 0x09, 0x02, 0x0B, 0x0C, 0x0C, 0x0C, 0x0F,     /* 0x68 to 0x6F */
    0x01, 0x01, 0x02, 0x01, 0x04, 0x01, 0x06, 0x0F,     /* 0x70 to 0x77 */
    0x08, 0x01, 0x0A, 0x0F, 0x0C, 0x0F, 0x0F, 0x0F      /* 0x78 to 0x7F */
};

// ------------------------------------------------------------------------------------ //

// helper function to get a specific bit from a byte
uint8_t get_bit(uint8_t byte, int position) {
    return (byte >> position) & 0x01;
}

// function to detect and correct errors in hamming code
void correct_error_with_lookup(uint8_t bytes[TOTAL_DISKS], int bit_pos, int debug) {
    // construct 7-bit hamming code from the bytes
    uint8_t hamming_code = 0;
    
    
    hamming_code |= get_bit(bytes[0], bit_pos) << 2; // Custom parity
    hamming_code |= get_bit(bytes[1], bit_pos);      // bit 0
    hamming_code |= get_bit(bytes[2], bit_pos) << 6; // bit 6
    hamming_code |= get_bit(bytes[3], bit_pos) << 1; // bit 1
    hamming_code |= get_bit(bytes[4], bit_pos) << 5; // bit 5 
    hamming_code |= get_bit(bytes[5], bit_pos) << 4; // bit 4
    hamming_code |= get_bit(bytes[6], bit_pos) << 3; // bit 3
    
    // calculate parity result to check for errors
    uint8_t parity_res = 0;
    
    // check P1 parity for D1, D2, D4
    if ((get_bit(bytes[2], bit_pos) ^ get_bit(bytes[4], bit_pos) ^ 
         get_bit(bytes[6], bit_pos)) != get_bit(bytes[0], bit_pos)) {
        parity_res |= 1;
    }
    
    // check P2 parity for D1, D3, D4
    if ((get_bit(bytes[2], bit_pos) ^ get_bit(bytes[5], bit_pos) ^ 
         get_bit(bytes[6], bit_pos)) != get_bit(bytes[1], bit_pos)) {
        parity_res |= 2;
    }
    
    // check P4 parity for D2, D3, D4
    if ((get_bit(bytes[4], bit_pos) ^ get_bit(bytes[5], bit_pos) ^ 
         get_bit(bytes[6], bit_pos)) != get_bit(bytes[3], bit_pos)) {
        parity_res |= 4;
    }
    
    // if error detected, fix the bit in the appropriate file
    if (parity_res != 0) {
        uint8_t error_bit = error_pos[parity_res];
        
        if (debug) {
            printf("Error detected at bit position %d, parity_res %d, error bit %d\n", 
                   bit_pos, parity_res, error_bit);
        }
        
        // flip the bit to correct the error
        switch (error_bit) {
            case 1: bytes[0] ^= (1 << bit_pos); break; // P1
            case 2: bytes[1] ^= (1 << bit_pos); break; // P2
            case 3: bytes[2] ^= (1 << bit_pos); break; // D1
            case 4: bytes[3] ^= (1 << bit_pos); break; // P4
            case 5: bytes[4] ^= (1 << bit_pos); break; // D2
            case 6: bytes[5] ^= (1 << bit_pos); break; // D3
            case 7: bytes[6] ^= (1 << bit_pos); break; // D4
        }
    }
}

// extract data nibble from hamming code
uint8_t extract_data_bits(uint8_t bytes[TOTAL_DISKS], int bit_pos) {
    uint8_t data = 0;
    // D1, D2, D3, D4 based on encoding
    data |= get_bit(bytes[2], bit_pos) << 3; // bit 6 -> D1
    data |= get_bit(bytes[4], bit_pos) << 2; // bit 5 -> D2
    data |= get_bit(bytes[5], bit_pos) << 1; // bit 4 -> D3
    data |= get_bit(bytes[6], bit_pos);      // bit 3 -> D4
    return data;
}

int main(int argc, char *argv[]) {
    char *inFile = NULL;
    int fileSize = 0;
    int debug = 0;
    int opt;
    FILE *inputs[TOTAL_DISKS];
    char filename[256];
    char outFilename[256];
    
    // parse command line arguments to get input file and size
    while ((opt = getopt(argc, argv, "f:s:d")) != -1) {
        switch (opt) {
            case 'f':
                inFile = optarg;
                break;
            case 's':
                fileSize = atoi(optarg);
                break;
            case 'd':
                debug = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s -f filename -s filesize [-d]\n", argv[0]);
                exit(1);
        }
    }
    
    // validate required arguments
    if (inFile == NULL || fileSize <= 0) {
        fprintf(stderr, "Error: Input file (-f) and file size (-s) are required\n");
        fprintf(stderr, "Usage: %s -f filename -s filesize [-d]\n", argv[0]);
        exit(1);
    }

    if (debug) {
        printf("Input file base: %s\n", inFile);
        printf("File size: %d bytes\n", fileSize);
    }

    // open all seven part files for reading
    for (int i = 0; i < TOTAL_DISKS; i++) {
        sprintf(filename, "%s.part%d", inFile, i);
        inputs[i] = fopen(filename, "rb");
        if (!inputs[i]) {
            perror("Error opening input file");
            // close files that were opened
            for (int j = 0; j < i; j++) {
                fclose(inputs[j]);
            }
            exit(1);
        }
        if (debug) {
            printf("Opened: %s\n", filename);
        }
    }

    // open output file for writing
    sprintf(outFilename, "%s.2", inFile);
    FILE *output = fopen(outFilename, "wb");
    if (!output) {
        perror("Error opening output file");
        for (int i = 0; i < TOTAL_DISKS; i++) {
            fclose(inputs[i]);
        }
        exit(1);
    }
    
    if (debug) {
        printf("Output file: %s\n", outFilename);
    }

    // array to hold bytes from part files and process them
    uint8_t part_bytes[TOTAL_DISKS];
    
    // read first byte from each part file
    for (int i = 0; i < TOTAL_DISKS; i++) {
        part_bytes[i] = fgetc(inputs[i]);
    }
    
    // process each byte to reconstruct the original file
    int bytes_processed = 0;

    while (bytes_processed < fileSize) {
        // each bit position across the 7 files gives us one Hamming code with 4 data bits
        
        for (int bit_pos = 7; bit_pos >= 0; bit_pos--) {
    
            correct_error_with_lookup(part_bytes, bit_pos, debug);
                        uint8_t nibble = extract_data_bits(part_bytes, bit_pos);
            
            // every two nibbles, combine them into a byte and write it
            static uint8_t high_nibble = 0;
            if (bit_pos % 2 == 1) { 
                high_nibble = nibble;
            } else {
                uint8_t reconstructed_byte = (high_nibble << 4) | nibble;
                fputc(reconstructed_byte, output);
                bytes_processed++;
                                
                // check if we've processed enough bytes
                if (bytes_processed >= fileSize) {
                    break;
                }
            }
        }
        
        // Read the next byte from each part file
        for (int i = 0; i < TOTAL_DISKS; i++) {
            if (!feof(inputs[i])) {
                part_bytes[i] = fgetc(inputs[i]);
            }
        }
    }
    
    if (debug) {
        printf("decoding complete processed %d bytes\n", bytes_processed);
    }
    
    // cleanup by closing all files
    fclose(output);
    for (int i = 0; i < TOTAL_DISKS; i++) {
        fclose(inputs[i]);
    }
    
    return 0;
}