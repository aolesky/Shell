#include <stdlib.h> 
#include "linked-list.h"
#include "command-block.h"
#include "load-program.h"
#include "piping.h"


int find_piping(LinkedList *input_command, bool *is_pipe, LinkedList *first_command_list, LinkedList *second_command_list) {
	*is_pipe = false;
	while (get_length(input_command) > 0) {
		char *word = pop_front(input_command);
		if (word[0] == '|') {
			*is_pipe = true;
			free(word);
			break;
		} else {
			push_back(first_command_list, word);
			free(word);
		}
	}

		//Get second comand in piping
	while (get_length(input_command) > 0) {
		char *word = pop_front(input_command);

		if (word[0] == '|') {
			free(word);
			return 0;
		} else {
			push_back(second_command_list, word);
			free(word);
		}
	}

	return 1;
}

int check_pipe(CommandBlock *first_command, CommandBlock *second_command) {

	if (second_command->command == NULL) {
		return 0;
	}

	if (first_command->output != NULL){
		return 0;
	}

	return 1;
}
