# Custom Bash/ Shell Implementation
Sawyer Bowerman

### March 28, 2025
I began with:
- Adding global variables for the copy of environment variables and count of env variables, as well ascommand history and count of history 
- Added lookup env helper function to find the environment variable which returns the index of the variable otherwise returns -1.
- I also added a helper function to add any commands to history.
- I then added a block of code in the main function to initialize the env variables.

### March 28, 2025 Pt.2
- Added logic for command env
- Added logic for command setenv with all the logic for allocating memory and replacing old assignments.
- Added logic for unsetenv and removing old env var from array.
- Added logic for history by calculating the number of entries and where to start. then printing out the history of commands.
I skipped cd for now because I was pretty unsure about how to go about doing it for the time being.

for (i = 0; i < env_count; i++) {
    free(env_copy[i]);
  } 

^^^ I also added this block of code to free up the memory before exiting main.

### March 28, 2025 Pt.3
- For the cd command, I implemented directory navigation functionality that allows users to change directories. The implementation uses the chdir() system call to change directories. After changing directories, it updates the PWD environment variable using getcwd() to reflect the new working directory.
- Implemented the ability to run external commands by searching through the PATH environment variable. When a user enters a command that isn't built-in, the shell looks through each directory listed in PATH to find an executable matching the command name. If found, it forks a child process and executes the command using execv().

- I added support for input and output redirection using the '<' and '>' operators. The implementation scans the command arguments for these redirection symbols, opens the specified files, and uses the dup2() system call to connect them to standard input or output. This allows users to read command input from files and save command output to files, making the shell much more powerful for file processing tasks.

- I enhanced the command parser to handle commands without spaces around special characters like '|', '<', and '>'. The improved parser inserts spaces around these operators during processing, allowing users to type commands in a more concise way like 'cat file.txt|grep pattern>output.txt'.

- I made a typescript displaying functionality of extra credit problems 1-3

Just to let the reader know, if the date modified times look odd, its because I just added everything to a git repo, you can check for yourself

https://github.com/shplok/cs444

### Resources
https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
https://www.man7.org/linux/man-pages/man3/getcwd.3.html
https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
https://linux.die.net/man/3/execv
https://www.man7.org/linux/man-pages/man0/fcntl.h.0p.html
https://pubs.opengroup.org/onlinepubs/7908799/xsh/fcntl.h.html
https://stackoverflow.com/questions/24538470/what-does-dup2-do-in-c