#ifndef SEA_SHELL_H
#define SEA_SHELL_H

// Basic declarations
void ssh_loop();
char* ssh_read_line();
char** ssh_split_line(char* line);
int ssh_launch(char** args);
int ssh_execute(char** args);

// Built-in commands
int ssh_cd(char** args);
int ssh_help(char** args);
int ssh_exit(char** args);


#endif // !SEA_SHELL_H
