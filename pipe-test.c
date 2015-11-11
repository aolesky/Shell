#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "linked-list.h"
#include "command-block.h"
#include "tokenizer.h"

#define STD_OUT      1


int main( int argc, char *argv[])
{
	TOKENIZER *tokenizer;
	char string[256] = "";
	char *tok;
	int br;

	string[255] = '\0';	   /* ensure that string is always null-terminated */
	write(STD_OUT,  "\n\nBlock to test: ", 17);

	br = read( STDIN_FILENO, string, 255 );
	string[br-1] = '\0';
	tokenizer = init_tokenizer( string );

	LinkedList *list = new_list(256);

	while( (tok = get_next_token( tokenizer )) != NULL ) {
		push_back(list, tok);
		free(tok);
	}
	free_tokenizer( tokenizer );

		// First Scan the input for pipes
	LinkedList *first_command = new_list(50);
	LinkedList *second_command = NULL;
	bool is_pipe = false;
	while (get_length(list) > 0) {
		char *word = pop_front(list);
		if (word[0] == '|') {
			is_pipe = true;
			free(word);
			break;
		} else {
			push_back(first_command, word);
			free(word);
		}
	}
	if (is_pipe) {
			//Handle case when nothing follows the pipe
		if (get_length(list) == 0) {
			printf("\nInvalid Piping, must be second command\n");
			free(list);
			return 1;
		}
		second_command = new_list(50);

		bool next_command = false;
		while (get_length(list) > 0) {
			char *word = pop_front(list);

			if (word[0] == '|') {
				free(word);
				printf("Only one pipe supported\n"); 
				next_command = true;
				break;
			} else {
				push_back(second_command, word);
				free(word);
			}
		}
		if (next_command) {
			write(1,  "\nmy-sh$ ", 8);
			return 0;
		}
	}


	destroy_list(list);


	print_block(new_command_block(first_command));
	print_block(new_command_block(second_command));

	return 0;
}
