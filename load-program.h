#ifndef LOAD_PROGRAM_H
#define LOAD_PROGRAM_H

int open_descriptors(char* input, char* output);

int change_descriptors(int input, int output);

int execute_process (CommandBlock *command, bool is_foreground);

int handle_fork(LinkedList *command, bool is_foreground);

void exec_command(LinkedList *command);

int execute_pipe(CommandBlock *first_command, CommandBlock *second_command, bool should_wait);

void wait_process(int gpid, int *status);


#endif