#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//accept up to 16 command-line arguments
#define MAXARG 16

//allow up to 64 environment variables
#define MAXENV 64

//keep the last 500 commands in history
#define HISTSIZE 500

//accept up to 1024 bytes in one command
#define MAXLINE 1024

static char **parseCmd(char cmdLine[]) {
  char **cmdArg, *ptr;
  int i;

  //(MAXARG + 1) because the list must be terminated by a NULL ptr
  cmdArg = (char **) malloc(sizeof(char *) * (MAXARG + 1));
  if (cmdArg == NULL) {
    perror("parseCmd: cmdArg is NULL");
    exit(1);
  }
  for (i = 0; i <= MAXARG; i++) //note the equality
    cmdArg[i] = NULL;
  i = 0;
  ptr = strsep(&cmdLine, " ");
  while (ptr != NULL) {
    // (strlen(ptr) + 1)
    cmdArg[i] = (char *) malloc(sizeof(char) * (strlen(ptr) + 1));
    if (cmdArg[i] == NULL) {
      perror("parseCmd: cmdArg[i] is NULL");
      exit(1);
    }
    strcpy(cmdArg[ i++ ], ptr);
    if (i == MAXARG)
      break;
    ptr = strsep(&cmdLine, " ");
  }
  return(cmdArg);
}

int main(int argc, char *argv[]) {
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
  while (( 1 )) {
    printf("bsh> ");                      //prompt
    fgets(cmdLine, MAXLINE, stdin);       //get a line from keyboard
    cmdLine[strlen(cmdLine) - 1] = '\0';  //strip '\n'
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
    }
    //built-in command setenv
    else if (strcmp(cmdArg[0], "setenv") == 0) {
    }
    //built-in command unsetenv
    else if (strcmp(cmdArg[0], "unsetenv") == 0) {
    }
    //built-in command cd
    else if (strcmp(cmdArg[0], "cd") == 0) {
    }
    //built-in command history
    else if (strcmp(cmdArg[0], "history") == 0) {
    }

    //implement how to execute Minix commands here

    //the following is a template for using fork() and execv()
    //***remove this else case from your final code for bsh.c
    else {
      if (debug)
	printf("calling fork()\n");
      pid = fork();
      if (pid != 0) {
	if (debug)
	  printf("parent %d waiting for child %d\n", getpid(), pid);
	waitpid(pid, &status, 0);
      }
      else {
	status = execv(cmdArg[0], cmdArg);
	if (status) {
	  printf("\tno such command (%s)\n", cmdArg[0]);
	  return 1;
	}
      }
    }
    //***remove up to this point

    //clean up before running the next command
    i = 0;
    while (cmdArg[i] != NULL)
      free( cmdArg[i++] );
    free(cmdArg);
  }

  return 0;
}
