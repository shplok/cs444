#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <fcntl.h>

//accept up to 16 command-line arguments
#define MAXARG 16

//allow up to 64 environment variables
#define MAXENV 64

//keep the last 500 commands in history
#define HISTSIZE 500

//accept up to 1024 bytes in one command
#define MAXLINE 1024


char *env_copy[MAXENV]; // copy of environment variables
int env_count = 0;      // count of environment variables

char history[HISTSIZE][MAXLINE]; // command history
int hist_count = 0;              // count of commands in history

// Returns index if found, -1 if not found
int find_env(const char *var) {
  int len = strlen(var);
  
  for (int i = 0; i < env_count; i++) {
      if (strncmp(env_copy[i], var, len) == 0 && env_copy[i][len] == '=') {
          return i;
      }
  }
  return -1;
}

// add a command to history
void add_to_history(char *cmd) {
  if (strlen(cmd) > 0) {
      int idx = hist_count % HISTSIZE;
      strncpy(history[idx], cmd, MAXLINE-1);
      history[idx][MAXLINE-1] = '\0';
      hist_count++;
  }
}

static char **parseCmd(char cmdLine[]) {
  char **cmdArg, *ptr;
  int i, j, k;
  char expandedCmd[MAXLINE * 2];

  // Insert spaces around special characters (|, <, >)
  j = 0;
  for (i = 0; cmdLine[i] != '\0' && j < MAXLINE * 2 - 1; i++) {
    if (cmdLine[i] == '|' || cmdLine[i] == '<' || cmdLine[i] == '>') {
      // put spaces before special character if not already a space
      if (i > 0 && cmdLine[i-1] != ' ' && j < MAXLINE * 2 - 1) {
        expandedCmd[j++] = ' ';
      }
      // add the special character
      expandedCmd[j++] = cmdLine[i];
      // add space after special character if not already there
      if (cmdLine[i+1] != ' ' && cmdLine[i+1] != '\0' && j < MAXLINE * 2 - 1) {
        expandedCmd[j++] = ' ';
      }
    } else {
      expandedCmd[j++] = cmdLine[i];
    }
  }
  expandedCmd[j] = '\0';

  //(MAXARG + 1) because the list must be terminated by a NULL ptr
  cmdArg = (char **) malloc(sizeof(char *) * (MAXARG + 1));
  if (cmdArg == NULL) {
    perror("parseCmd: cmdArg is NULL");
    exit(1);
  }
  for (i = 0; i <= MAXARG; i++) //note equality
    cmdArg[i] = NULL;
  i = 0;
  
  // call the spaced command
  char *cmdLineCopy = strdup(expandedCmd);
  ptr = strsep(&cmdLineCopy, " ");
  while (ptr != NULL) {
    if (strlen(ptr) > 0) {  // Skip empty tokens
      cmdArg[i] = (char *) malloc(sizeof(char) * (strlen(ptr) + 1));
      if (cmdArg[i] == NULL) {
        perror("parseCmd: cmdArg[i] is NULL");
        exit(1);
      }
      strcpy(cmdArg[i++], ptr);
      if (i == MAXARG)
        break;
    }
    ptr = strsep(&cmdLineCopy, " ");
  }
  free(cmdLineCopy);
  return(cmdArg);
}


void process_redirection(char **cmdArg) {
  int i, in_redirect = -1, out_redirect = -1;
  
  // Find redirection symbols
  for (i = 0; cmdArg[i] != NULL; i++) {
    if (strcmp(cmdArg[i], "<") == 0) {
      in_redirect = i;
    } else if (strcmp(cmdArg[i], ">") == 0) {
      out_redirect = i;
    }
  }
  
  // input redirection
  if (in_redirect != -1) {
    if (cmdArg[in_redirect + 1] == NULL) {
      fprintf(stderr, "Error: Missing input file\n");
      return;
    }
    
    int fd = open(cmdArg[in_redirect + 1], O_RDONLY);
    if (fd < 0) {
      perror("open input file");
      return;
    }
    
    // redirect stdin
    dup2(fd, STDIN_FILENO);
    close(fd);
    
    // Remove redirection from arguments
    cmdArg[in_redirect] = NULL;
  }
  
  // Handle output redirection
  if (out_redirect != -1) {
    if (cmdArg[out_redirect + 1] == NULL) {
      fprintf(stderr, "Error: Missing output file\n");
      return;
    }
    
    int fd = open(cmdArg[out_redirect + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
      perror("open output file");
      return;
    }
    
    // Redirect stdout
    dup2(fd, STDOUT_FILENO);
    close(fd);
    
    // Remove redirection from arguments
    cmdArg[out_redirect] = NULL;
  }
}

int main(int argc, char *argv[], char *envp[]) {
  char cmdLine[MAXLINE], **cmdArg;
  int status, i, debug;
  pid_t pid;

  debug = 1;
  i = 1;
  while (i < argc) {
    if (! strcmp(argv[i], "-d") )
      debug = 1;
    i++;
  }

  // Initialize environment variables
  for (i = 0; envp[i] != NULL && env_count < MAXENV; i++) {
    env_copy[env_count] = strdup(envp[i]);
    if (env_copy[env_count] == NULL) {
        perror("Failed to copy environment variable");
        exit(1);
    }
    env_count++;
  }

  while (( 1 )) {
    printf("bsh> ");                      //prompt
    fgets(cmdLine, MAXLINE, stdin);       //get a line from keyboard
    cmdLine[strlen(cmdLine) - 1] = '\0';  //strip '\n'

    add_to_history(cmdLine);

    cmdArg = parseCmd(cmdLine);
    if (debug) {
      i = 0;
      while (cmdArg[i] != NULL) {
	printf("\t%d (%s)\n", i, cmdArg[i]);
	i++;
      }
    }

    //built-in command exit
    if (strcmp(cmdArg[0], "exit") == 0) {
      if (debug)
	printf("exiting\n");
      break;
    }
    //built-in command env
    else if (strcmp(cmdArg[0], "env") == 0) {
      for (i = 0; i < env_count; i++) {
          printf("%s\n", env_copy[i]);
      }
    }
    //built-in command setenv
    else if (strcmp(cmdArg[0], "setenv") == 0) {
      if (cmdArg[1] == NULL || cmdArg[2] == NULL) {
          printf("Usage: setenv variable value\n");
      } else {
          char *new_var;
          int idx = find_env(cmdArg[1]);
          
          new_var = malloc(strlen(cmdArg[1]) + strlen(cmdArg[2]) + 2); // +2 for '=' and '\0'
          if (new_var == NULL) {
              perror("setenv: malloc failed");
              continue;
          }
          sprintf(new_var, "%s=%s", cmdArg[1], cmdArg[2]);
          
          if (idx >= 0) {
              // replace var
              free(env_copy[idx]);
              env_copy[idx] = new_var;
          } else if (env_count < MAXENV) {
              // new variable
              env_copy[env_count++] = new_var;
          } else {
              printf("Environment is full, cannot add new variable\n");
              free(new_var);
          }
      }
    }
    //built-in command unsetenv
    else if (strcmp(cmdArg[0], "unsetenv") == 0) {
      if (cmdArg[1] == NULL) {
          printf("Usage: unsetenv variable\n");
      } else {
          int idx = find_env(cmdArg[1]);
          if (idx >= 0) {
              free(env_copy[idx]);
              // Shift remaining variables down
              for (int j = idx; j < env_count - 1; j++) {
                  env_copy[j] = env_copy[j + 1];
              }
              env_count--;
          } else {
              printf("Variable %s not found\n", cmdArg[1]);
          }
      }
    }
    //built-in command cd
    else if (strcmp(cmdArg[0], "cd") == 0) {
      char cwd[MAXLINE];
      
      // Handle empty path or just "cd" - go home
      if (cmdArg[1] == NULL || strlen(cmdArg[1]) == 0) {
          // Find HOME in environment variables
          int idx = find_env("HOME");
          if (idx >= 0) {
              char *home = strchr(env_copy[idx], '=') + 1; // Skip "HOME="
              if (chdir(home) != 0) {
                  perror("cd");
              }
          } else {
              printf("HOME not set\n");
          }
      } else {
          // Change to specified directory
          if (chdir(cmdArg[1]) != 0) {
              perror("cd");
          }
      }
      
      // Update PWD environment variable
      if (getcwd(cwd, MAXLINE) != NULL) {
          int idx = find_env("PWD");
          char *new_pwd = malloc(strlen("PWD=") + strlen(cwd) + 1);
          if (new_pwd == NULL) {
              perror("cd: malloc failed");
              continue;
          }
          sprintf(new_pwd, "PWD=%s", cwd);
          
          if (idx >= 0) {
              free(env_copy[idx]);
              env_copy[idx] = new_pwd;
          } else if (env_count < MAXENV) {
              env_copy[env_count++] = new_pwd;
          } else {
              printf("Environment is full, cannot update PWD\n");
              free(new_pwd);
          }
      }
    }

    //built-in command history
    else if (strcmp(cmdArg[0], "history") == 0) {
      int start, entries;
      
      // Calculate how many entries to show and the starting point
      entries = (hist_count < HISTSIZE) ? hist_count : HISTSIZE;
      start = (hist_count <= HISTSIZE) ? 0 : hist_count - HISTSIZE;
      
      for (i = 0; i < entries; i++) {
          int idx = (start + i) % HISTSIZE;
          printf("%d: %s\n", start + i + 1, history[idx]);
      }
    }

    //implement how to execute Minix commands here

    // Replace the template else block with this:
    else {
      // This code runs when the command is not a built-in
      int found = 0;
      char full_path[MAXLINE];
      
      // Try to find the command in PATH
      int path_idx = find_env("PATH");
      if (path_idx >= 0) {
        char *path_copy = strdup(env_copy[path_idx]);
        if (path_copy == NULL) {
          perror("malloc failed for PATH copy");
          continue;
        }
        
        char *path_var = strchr(path_copy, '=') + 1; // Skip "PATH="
        char *path_token = strsep(&path_var, ":");
        
        while (path_token != NULL && !found) {
          snprintf(full_path, MAXLINE, "%s/%s", path_token, cmdArg[0]);
          
          // Check if the file exists and is executable
          if (access(full_path, X_OK) == 0) {
            found = 1;
            
            if (debug)
              printf("Found command at %s\n", full_path);
            
            // Now execute the command
            pid = fork();
            if (pid == 0) { // child process!
              process_redirection(cmdArg);
              execv(full_path, cmdArg);
              perror("execv failed");
              exit(1);
            } else if (pid > 0) { // parent process!
              if (debug)
                printf("parent %d waiting for child %d\n", getpid(), pid);
              waitpid(pid, &status, 0);
            } else {
              perror("fork failed");
            }
          }
          path_token = strsep(&path_var, ":");
        }
        free(path_copy);
      }
      if (!found) {
        printf("Command not found: %s\n", cmdArg[0]);
      }
    }

    //clean up before running the next command
    i = 0;
    while (cmdArg[i] != NULL)
      free( cmdArg[i++] );
    free(cmdArg);
  }
  for (i = 0; i < env_count; i++) {
    free(env_copy[i]);
  } 
  return 0;
}
