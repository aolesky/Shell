#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "support-functions.h"
#include "tokenizer.h"
#include "linked-list.h"
#include "command-block.h"
#include "process-map.h"
#include "load-program.h"
#include "piping.h"
#include "handle-job.h"


bool determine_background(LinkedList *input_command);
int job_control (LinkedList *input_command, ProcessMap *jobs, int *most_recent_job);

/**
* Main program execution
*/
int main (int argc, char *argv[]) {
	TOKENIZER *tokenizer;
	char string[1024] = "";
	char *tok;
	int br;

	int most_recent_job = 0;

	ProcessMap *jobs = new_map();

	//Set up signal handling
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTERM, SIG_IGN);


	string[1023] = '\0';	   /* ensure that string is always null-terminated */
	printf("\nEnter a command or type ctrl-d to end session.\n" );
	write(1, "\nmy-sh$ ", 8);

	//Input loop
	while ((br = read( STDIN_FILENO, string, 1023 )) > 0) {

		if (br <= 1) {
			write(1, "my-sh$ ", 8);
			continue;
		}

		string[br-1] = '\0';
		tokenizer = init_tokenizer(string);

		//Create linked list of tokens
		LinkedList *input_command = new_list(256);
		while( (tok = get_next_token( tokenizer )) != NULL ) {
			push_back(input_command, tok);
			free(tok);
		}
		free_tokenizer(tokenizer);


		int executed = 0;
		int error = 0;
		//Checks for fg or bg
		if (get_length(input_command) == 1) {
			char *only_token = pop_back(input_command);

			if (compare_strings(only_token, "fg")) {
				if (move_to_foreground(jobs, &most_recent_job) == -1)
					error = 1;
				executed = 1;
					
			} else if (compare_strings(only_token, "bg")) {
				if (move_to_background(jobs, &most_recent_job) == -1)
					error = 1;
				executed = 1;
			} else {
				push_back(input_command, only_token);
			}
			free(only_token);
		}


		//Process input for pipes or background if an error has already been detected, go to the next command
		if (!executed && !error) {

			//Sees if a background ampersand is detected
			bool is_background = determine_background(input_command);

			LinkedList *full_command = copy_list(input_command);

			if (is_background) {
				printf("Running: ");
				print_list(input_command);
			}

			//Test for pipes
			bool is_pipe = false;
			LinkedList *first_command_list = new_list(50);
			LinkedList *second_command_list = new_list(50);
			int valid_pipe = find_piping(input_command, &is_pipe, first_command_list, second_command_list);

			//Command blocks are created from the command lists
			CommandBlock *first_command = new_command_block(first_command_list);
			CommandBlock *second_command = new_command_block(second_command_list);

			//Runs a function to check that there are no invalid redirections in the case of a piping
			if (is_pipe) {
				valid_pipe = valid_pipe && check_pipe(first_command, second_command);
			}

			//Notifies user of any incorrect pipe commands
			if (!is_pipe && !first_command->valid) {
				printf("Invalid command structure\n");
			} else if (is_pipe && (!first_command->valid || !second_command->valid || !valid_pipe) ) {
				printf("Invalid command structure\n");
			}


			//If it is a pipe and all necessary conditions are valid, then the piping occurs
			if (is_pipe && first_command->valid && second_command->valid && valid_pipe) {
				
				if (pipe_job (first_command, second_command, is_background, full_command, jobs, &most_recent_job) 
					== -1)
					error = 1;

			} 
			// No piping
			else if (!is_pipe && first_command->valid) {

				if (job (first_command, is_background, full_command, jobs, &most_recent_job) == -1)
					error = 1;
			}

			destroy_list(first_command_list);
			destroy_list(second_command_list);
			destroy_block(first_command);
			destroy_block(second_command);
			destroy_list(full_command);
		}

		destroy_list(input_command);

		
		monitor_background_jobs (jobs, &most_recent_job);

		if (error)
			perror("ERROR ");

		write(1, "my-sh$ ", 8);
	}

	destroy_map(jobs);

	
	
	printf( "\nSession ended\n" );
	return 0;
}


bool determine_background(LinkedList *input_command) {
	bool is_background;
		//Now see if this should run in background
	char *last_token = pop_back(input_command);

	if (last_token[0] == '&') {
		is_background = true;
	} else {
		is_background = false;
		push_back(input_command, last_token);
	}

	free(last_token);

	return is_background;
}


