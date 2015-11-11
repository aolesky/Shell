#ifndef COMMAND_BLOCK_H
#define COMMAND_BLOCK_H

typedef int bool;
#define true 1
#define false 0

typedef struct {
	LinkedList *command;
	char *input;
	char *output;
	bool valid;
} CommandBlock;


CommandBlock *new_command_block(LinkedList *tokens);
int parse_command(CommandBlock *result, LinkedList *tokens);
int parse_output(CommandBlock *result, LinkedList *tokens);
int parse_input(CommandBlock *result, LinkedList *tokens);
void print_block(CommandBlock *block);

bool is_valid(CommandBlock *block);
void destroy_block(CommandBlock *block);



#endif
