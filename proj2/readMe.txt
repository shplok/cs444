# Hamming Encoding & Decoding Implementation
Sawyer Bowerman

### February 28, 2025
- Created both RAID and DIAR files
- Copied Dr.Deblois' code to my proj2 dir
- Created an archive file as well as a file for the professors files if needed.
- Created Main Function and set up the -f flag and debug.

### February 28, 2025 pt.2
- Updated output to consider the 7 output files.
- Added a buffer for the bit operations
- added support for reading the input file line by line
- Added lookup tables for the possible hamming(7,4) data pattern
- Started adding support for adding bits to the buffers as well as flushing the buffers.
- Added some logic to clean up the memory that was allocated.
- Added what I thought the error correction would possibly be to diarsbb.c
- Updated Debug Statements to provide important info
- Fixed mem free issues.

### March 2, 2025
- Implemented Add to Buffer function
- Implemented Function to flush the buffers after processing is complete
- Update bit distribution logic
- currently running into issues where things are being compiled to the incorrect file parts.
- I need to follow this pattern: (possibly?)

xyz.part0 - P1 bits
xyz.part1 - P2 bits
xyz.part2 - D1 bits
xyz.part3 - P4 bits
xyz.part4 - D2 bits
xyz.part5 - D3 bits
xyz.part6 - D4 bits

to return a proper diff. My proposed solution is to call each function call inline but I don't think my current loop is allowing me to match files to jhd.

- After trying a good bit, I managed to match 6/7 to the raidjhd output. I'm going to call it for now since that was pretty taxing.

### March 4, 2025
- I just had an epiphany, realizing that it didn't really matter what I output in which order of parts for raid because the grader is just going to check the final state... Wait, of course it has to be the only one that I don't have correct.
- Ill need to add that to my TODO then.
- Began Diar implementation today, Added the getopt flags for parsing the command.
- Added more error handling for diarsbb and begun the actual code logic.
- Added a TODO list to the file to make sure I'm staying on track.
 
Side Note: I don't want to do any additional work on diar until I figure out this silly issue in raid where my bits aren't calculating a correct parity 
- Added more debug statements
UPDATE : Lost all progress on diar because ssh wasn't saving for some odd reason, will start from ground up later.
- Final Updater for this portion: I feel a little stupid to not have noticed that ssh wasn't updating. This being said, I got it!! I had to handle the logic for the first parity bit in a different way but i got it to exactly match the provided output, so that's a huuuge relief.

soyr@pe15:~/cs444/proj2$ for i in {0..6}; do   echo "Hexdump comparison for part$i:";   diff <(hexdump -C my_output/test.txt.part$i) <(hexdump -C prof_output/test.txt.part$i) | head; done
Hexdump comparison for part0:
Hexdump comparison for part1:
Hexdump comparison for part2:
Hexdump comparison for part3:
Hexdump comparison for part4:
Hexdump comparison for part5:
Hexdump comparison for part6:

### March, 5, 2024 
- Began implementation on Diar by figuring out the reverse mapping for the hamming decode table.
- Added more robust support for opening the 7 files
- Finally utilized the lookup tables for more efficient error detection
- Updated error handling and debug statements
- Handled file I/O operations properly, including opening multiple part files and creating the output file for the reconstructed data.
- At a point where diar is doing something but definitely not something correctly yet

### March, 5 2025
- I hadn't realized but the effort I took to make sure that the output of raid lined up with Dr. Deblois also needed to be considered in diar. 

It had slipped my mind but when I changed the bit orders, and some structure of the byte processing logic, I was able to return no diff on test.txt

- About to test with completeShakespeare

soyr@pe15:~/cs444/proj2$ ./diarsbb -f completeShakespeare.txt -s 5694072
soyr@pe15:~/cs444/proj2$ ls
'archive(old files)'             completeShakespeare.txt.part3   diarsbb       raidsbb
 completeShakespeare.txt         completeShakespeare.txt.part4   diarsbb.c     raidsbb.c
 completeShakespeare.txt.2       completeShakespeare.txt.part5   Makefile      readMe.txt
 completeShakespeare.txt.part0   completeShakespeare.txt.part6   my_output     test.txt
 completeShakespeare.txt.part1  'copiedFiles(prof'\''s)'         prof_output
 completeShakespeare.txt.part2   diarjhd                         raidjhd
soyr@pe15:~/cs444/proj2$ diff completeShakespeare.txt completeShakespeare.txt.2

No Diff! I think we're golden for now, if I can think of anything else, I'll implement but I think I did it!

### Resources
https://en.wikipedia.org/wiki/Hamming_code
https://en.wikipedia.org/wiki/Hamming(7,4)
https://en.wikipedia.org/wiki/Standard_RAID_levels
https://github.com/MichaelDipperstein/hamming/blob/master/hamming.c=