#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "linked-list.h"
#include "command-block.h"
#include "load-program.h"

#define STDIN_FILENO	0
#define STDOUT_FILENO	1

// Changes descriptors if unopened
int open_descriptors(char* input, char* output) {

 	// Change input and output if specified
	if (output != NULL) {
		int new_out = open(output, O_WRONLY|O_CREAT, 0644);
		if (new_out == -1) {
			return -1;
		}
		if (dup2(new_out, STDOUT_FILENO) == -1)
			return -1;
	}
	if (input != NULL) {
		int new_in = open(input, O_RDONLY, 0);
		if (new_in == -1) {
			return -1;
		}
		if (dup2(new_in, STDIN_FILENO) == -1)
			return -1;
	}

	return 0;
}

// Changes descriptors if already open
int change_descriptors(int input, int output) {

	if (output >= 0) {
		if (dup2(output, STDOUT_FILENO) == -1)
			return -1;
	}
	if (input >= 0) {
		if (dup2(input, STDIN_FILENO) == -1)
			return -1;
	}

	return 0;
}

// returns child pid
int execute_process (CommandBlock *command, bool is_foreground) {

	int init_in = dup2(STDIN_FILENO, STDIN_FILENO);
	int init_out = dup2(STDOUT_FILENO, STDOUT_FILENO);

	if (init_in == -1 || init_out == -1)
		return -1;

	if (open_descriptors(command->input, command->output) == -1)
		return -1;

	int child_pid = handle_fork(command->command, is_foreground);

	if (child_pid == -1)
		return -1;

	if (change_descriptors(init_in, init_out) == -1)
		return -1;

	return child_pid;
}


// Returns the pid of the started process
// Returns -1 if failure
int handle_fork(LinkedList *command, bool is_foreground) {
	pid_t child_pid = fork();

	if (child_pid == -1)
		return -1;
	else if (child_pid == 0) {

		setpgid(0, getpid());
		if (is_foreground) {
			signal(SIGTTOU, SIG_IGN);
			if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1){
				perror("ERROR : ");
				exit(1);
			}
			signal(SIGTTOU, SIG_DFL);
			
		}

		signal(SIGINT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTERM, SIG_DFL);

		exec_command(command);
		exit(1);
	}

	if (is_foreground) {
		if (tcsetpgrp(STDIN_FILENO, getpgid(child_pid)) == -1)
			return -1;
		kill(child_pid, SIGCONT);
	}


	return child_pid;
}


// General purpose command for calling exec given a command in a linkedList
void exec_command(LinkedList *command) {
	int args_length = get_length(command) + 1;
	char *args[args_length];

	char *program = (char *) pop_front(command);
	args[0] = malloc(sizeof(program));
	strcpy(args[0], program);
	free(program);

	int i = 0;
	while (get_length(command) > 0) {
		char *argument = pop_front(command);
		args[i + 1] = malloc(sizeof(argument));
		strcpy(args[i + 1], argument);
		free(argument);
		i++;
	}
	args[args_length - 1] = NULL;

	if (execvp(args[0], args) == -1) {
		perror("Program failed with error ");
		exit(1);
	}
}

// Handles entire piping process
int execute_pipe(CommandBlock *first_command, CommandBlock *second_command, bool is_foreground) {

	int pipefd[2];

	if (pipe(pipefd) == -1)
		return -1;


	pid_t output_child = fork();
	if (output_child == -1)
		return -1;

	if (output_child == 0) {

		int new_in = -1;
		if (first_command->input != NULL) {
			new_in = open(first_command->input, O_RDONLY, 0);
			if (new_in == -1) {
				perror("ERROR ");
				exit(1);
			}
			dup2(new_in, STDIN_FILENO);
		}

		close(pipefd[0]);
		change_descriptors(new_in, pipefd[1]);

		if (setpgid(0, 0) == -1) {
			perror("ERROR ");
			exit(1);
		}
		if (is_foreground) {
			signal(SIGTTOU, SIG_IGN);
			tcsetpgrp(STDIN_FILENO, getpgid(0));
			signal(SIGTTOU, SIG_DFL);
		}

		signal(SIGINT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);

		exec_command(first_command->command);
		exit(1);
	}
	if (setpgid(output_child, 0) == -1) {
		return -1;
	}

	int process_group = getpgid(output_child);

	pid_t input_child = fork();

	if (input_child == -1) {
		kill(output_child, SIGKILL);
		return -1;
	}

	if (input_child == 0) {
	
		int new_out = -1;
		if (second_command->output != NULL) {
			new_out = open(second_command->output, O_WRONLY|O_CREAT, 0644);
			if (new_out == -1) {
				perror("Output redirection failed: ");
				exit(1);
			}
			dup2(new_out, STDOUT_FILENO);
		}

		close(pipefd[1]);
		change_descriptors(pipefd[0], new_out);

		setpgid(0, process_group);

		signal(SIGTTOU, SIG_DFL);
		signal(SIGINT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);

		exec_command(second_command->command);
		exit(1);
	}

	if (setpgid(input_child, process_group) == -1) {
		perror("Error changing gpid ");
		return -1;
	}

	close(pipefd[0]);
	close(pipefd[1]);

	return process_group;
}
