# Huffman Coding Implementation
Sawyer Bowerman

### February 7, 2025
- Initial implementation of core Huffman coding algorithm
- Implemented basic tree building based on GFG reference implementation
- Created simple printCodes function for testing.
- Basic test case with "test" string
- No memory management or error handling yet
- Successfully generated Huffman codes for simple input.

### February 9, 2025
- Added proper code storage functionality with HuffmanCode struct
- Implemented storeCodes function based on printCodes logic
- Added test case with longer string "this is a test string".
- Im plemented proper memory management with freeTree
- Added basic frequency counting for input.

### February 10, 2025
- Added command line argument parsing with getopt
- Implemented file I/O operations
- Added binary output capabilities with writeBits
- Extended to handle complete text files
- Finished up implementation and checked with diff against professor's files

### Feburary 11, 2025
- Renamed some files to fit professor's requirements
- Copied all of professors proj1 files to a new subdirectory
- Double-checked to make sure the debug flag was utilized in getopt for output.

### Feburary 13, 2025
- Didn't do anything too crazy, just updated some comments in minheap.c.

### Feburary 17, 2025
- Didn't do anything too crazy again, updated some comments in minheap.c to help explain 
implementation a little bit better

SOURCES:
https://www.w3schools.com/dsa/dsa_ref_huffman_coding.php
https://www.geeksforgeeks.org/huffman-coding-in-c/                         < was my primary source
https://cs.umb.edu/~hdeblois/s25-slides-huff.pdf
https://en.wikipedia.org/wiki/Huffman_coding
