#include <stdlib.h>
#include <stdio.h>

#include "support-functions.h"
#include "linked-list.h"
#include "command-block.h"


void print_block(CommandBlock *block) {

	if (!block->valid) {
		printf("Command block is invalid.\n");
		return;
	}

	printf("Command: ");
	ListNode *current = block->command->head;
	while (current != NULL) {
		printf(" %s", current->string);
		current = current->next;
	}
	printf("\n");
	printf("Input: %s\n", block->input);
	printf("Output: %s\n", block->output);
}

CommandBlock *new_command_block(LinkedList *tokens) {
	CommandBlock *result = malloc(sizeof(CommandBlock));
	result->valid = true;
	result->input = NULL;
	result->output = NULL;
	result->command = NULL;

	int success = parse_command(result, tokens);
	if (success == -1) {
		result->valid = false;
		return result;
	}
	
	while (get_length(tokens) > 0) {
		char *tok = (char *) pop_front(tokens);
		if (tok[0] == '>') {
			free(tok);
			if (parse_output(result, tokens) == -1) {
				result->valid = false;
				return result;
			}
		} else if (tok[0] == '<') {
			free(tok);
			if (parse_input(result, tokens) == -1) {
				result->valid = false;
				return result;
			}
		} else {
			free(tok);
			exit(1);
		}
	}

	return result;
}

int parse_command(CommandBlock *result, LinkedList *tokens) {

	//First check there isn't an incorrect structure of a leading redirect

	if (get_length(tokens) == 0) {
		return -1;
	}

	char *first = pop_front(tokens);
	if (first[0] == '>' || first[0] == '<') {
		free(first);
		return -1;
	}

	push_front(tokens, first);
	free(first);

	result->command = new_list(50);
	int i;
	for (i = 0; get_length(tokens) > 0; i++) {
		char *tok = (char *) pop_front(tokens);

		if (tok[0] == '>' || tok[0] == '<') {
			push_front(tokens, tok);
			free(tok);

			result->valid = true;

			return 0;
		}

		push_back(result->command, tok);
		free(tok);
	}

	return 0;
}

int parse_output(CommandBlock *result, LinkedList *tokens) {

	//Check if there already was an output read

	if (result->output != NULL) {
		return -1;
	}

	//Now check there isn't an incorrect structure of a leading redirect
	char *first = (char *) pop_front(tokens);
	if (first[0] == '>' || first[0] == '<') {
		free(first);
		return -1;
	}

	int i = 0;
	while (first[i] != '\0')
		i++;

	i++;

	result->output = malloc(i);
	copy_string(first, result->output, i);

	free(first);
	return 0;
}

int parse_input(CommandBlock *result, LinkedList *tokens) {

	//Check if there already was an output read

	if (result->input != NULL) {
		return -1;
	}

	//Now check there isn't an incorrect structure of a leading redirect
	char *first = (char *) pop_front(tokens);
	if (first[0] == '>' || first[0] == '<') {
		free(first);
		return -1;
	}

	int i = 0;
	while (first[i] != '\0')
		i++;

	i++;

	result->input = malloc(i);
	copy_string(first, result->input, i);

	free(first);
	return 0;
}


bool is_valid(CommandBlock *block) {
	return block->valid;
}

void destroy_block(CommandBlock *block) {

	if (block->command)
		destroy_list(block->command);
	free(block->input);
	free(block->output);

	free(block);
}
