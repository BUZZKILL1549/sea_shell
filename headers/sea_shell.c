#include "sea_shell.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <threads.h>
#include <unistd.h>
#include <sys/wait.h>

#define SSH_RL_BUFSIZE 1024
#define SSH_TOK_BUFSIZE 64
#define SSH_TOK_DELIM " \t\r\n\a"

// builtin shell command list

char* builtin_strs[] = { "cd", "help", "exit" };

int (*builtin_func[]) (char**) = { &ssh_cd, &ssh_help, &ssh_exit };

int ssh_num_builtins() {
  return sizeof(builtin_strs) / sizeof(char*);
}

// Impl

int ssh_cd(char** args) {
  if (args[1] == NULL) {
    fprintf(stderr, "ssh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("ssh");
    }
  }

  return 1;
}

int ssh_help(char** args) {
  printf("Sea Shell - BUZZKILL1549\n");
  printf("Use the man command for more information.\n");

  return 1; 
}

int ssh_exit(char** args) {
  return 0;
}


// shell launch definitions

void ssh_loop() {
  char* line;
  char** args;
  int stat;

  do {
    printf("> ");
    line = ssh_read_line();
    args = ssh_split_line(line);
    stat = ssh_execute(args);

    free(line);
    free(args);
  } while (stat);
}

char* ssh_read_line() {
  char* line = NULL;
  size_t bufsize = 0;

  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

char** ssh_split_line(char* line) {
  int bufsize = SSH_TOK_BUFSIZE, position = 0;
  char** toks = malloc(bufsize * sizeof(char*));
  char* tok;

  if (!toks) {
    fprintf(stderr, "ssh: alloc error\n");
    exit(EXIT_FAILURE);
  }

  tok = strtok(line, SSH_TOK_DELIM);

  while (tok != NULL) {
    toks[position] = tok;
    position++;
    
    if (position >= bufsize) {
      bufsize += SSH_TOK_BUFSIZE;
      toks = realloc(toks, bufsize * sizeof(char*));

      if (!toks) {
        fprintf(stderr, "ssh: alloc error\n");
        exit(EXIT_FAILURE);
      }
    }

    tok = strtok(NULL, SSH_TOK_DELIM);
  }

  toks[position] = NULL;
  return toks;
}

int ssh_launch(char** args) {
  pid_t pid, wpid;
  int stat;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("ssh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    perror("ssh");
  } else {
    do {
      wpid = waitpid(pid, &stat, WUNTRACED);
    } while (!WIFEXITED(stat) && !WIFEXITED(stat));
  }

  return 1;
}

int ssh_execute(char** args) {
  if (args[0] == NULL) {
    return 1;
  }

  for (int i = 0; i < ssh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_strs[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return ssh_launch(args);
}


