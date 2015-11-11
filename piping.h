#ifndef PIPING_H
#define PIPING_H

int find_piping(LinkedList *input_command, bool *is_pipe, LinkedList *first_command_list, LinkedList *second_command_list);

int check_pipe(CommandBlock *first_command, CommandBlock *second_command);

#endif