#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "support-functions.h"
#include "linked-list.h"
#include "command-block.h"
#include "process-map.h"
#include "load-program.h"
#include "piping.h"
#include "handle-job.h"


int pipe_job (CommandBlock *first_command, CommandBlock *second_command, bool is_background, LinkedList* full_job_command, ProcessMap *processes, int *most_recent_job) {

	//First the forking and exec is handled here
	int child_pid = execute_pipe(first_command, second_command, !is_background);
	if (child_pid == -1)
		return -1;
	int process_group = getpgid(child_pid);
	if (process_group == -1)
		return -1;

	int exit_status;

	// If it is a foreground process the correct waits must be followed
	if (!is_background) {

		if (waitpid(-process_group, &exit_status, WUNTRACED) == -1)
			return -1;

		if (WIFSTOPPED(exit_status)) {
			printf("\nStopped: ");
			add_process(processes, process_group, full_job_command, 2);
			LinkedList *temp = get_command(processes, process_group);
			print_list(temp);
			destroy_list(temp);
			*most_recent_job = process_group;
		}

		if (waitpid(-process_group, &exit_status, WUNTRACED) == -1)
			return -1;
		if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1)
			return -1;

		if (WIFSTOPPED(exit_status)) {
			printf("Forerground process ended by stop\n");
			printf("\nStopped: ");
			add_process(processes, process_group, full_job_command, 1);;

			LinkedList *temp = get_command(processes, process_group);
			print_list(temp);
			destroy_list(temp);
			*most_recent_job = process_group;
		}
	}
	//If it isn't a foreground process, the process only needs to be waited on and added to the process table 
	else {
		add_process(processes, process_group, full_job_command, 2);
	}

	return 0;
}

int job (CommandBlock *command, bool is_background, LinkedList* full_job_command, ProcessMap *processes, int *most_recent_job) {
	int child_pid = execute_process(command, !is_background);
	if (child_pid == -1)
		return -1;

	if (setpgid(child_pid, 0) == -1)
		return -1;
	int child_pgrp = getpgid(child_pid);
	
	if (child_pgrp == -1)
		return -1;


	if (!is_background) {
		int exit_status = -1;

		if (waitpid(child_pid, &exit_status, WUNTRACED) == -1)
			return -1;
		
		if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1)
			return -1;

		if (WIFSTOPPED(exit_status)) {
			printf("\nStopped: ");
			add_process(processes, child_pgrp, full_job_command, 1);
			LinkedList *temp = get_command(processes, child_pgrp);
			print_list(temp);
			destroy_list(temp);
			*most_recent_job = child_pgrp;
		}

	} else {
		add_process(processes, child_pgrp, full_job_command, 1);
	}
	return 0;
}

//Cycles through currently running processes to see if any have stopped or terminated
//Removes them from the job tracker if necessary
int monitor_background_jobs (ProcessMap *processes, int *most_recent_job) {
	int status;
	pid_t process;

	ProcessMapNode *current = processes->head;
	while (current != NULL) {

		ProcessMapNode *temp = current->next;
		//Keeps checking each job in case there is more than one process
		int current_pgid = current->pgid;
		while ((process = waitpid(-current_pgid, &status, WNOHANG | WUNTRACED)) > 0 ) {

			//If exited notify user of completed
			if (WIFEXITED(status)) {
				if (current->count == 1) {
					printf("Completed: ");
					LinkedList *process_command = get_command(processes, current_pgid);
					remove_pgid(processes, current_pgid);
					print_list(process_command);
					destroy_list(process_command);

					//No more recent job, thus set to 0
					if (current_pgid == *most_recent_job)
						*most_recent_job = 0;
				}
					//If the count of processes is greater than one, then simply decrement the number
				else {
					current->count = current->count - 1;
				}
			}
				// If job is stopped, simply notify user of the stoppage
			else if (WIFSTOPPED(status)) {
				printf("Stopped: ");
				LinkedList *process_command = get_command(processes, process);
				print_list(process_command);
				destroy_list(process_command);
			} else {
				printf("Unsure how to handle background signal\n");
			}
		}
		current = temp;
	}

	return 0;
}

int move_to_foreground(ProcessMap *jobs, int *most_recent_job) {
	

	if (*most_recent_job == 0) {
		printf("No recent process to restart\n");
		return 0;
	}

	if (tcsetpgrp(STDIN_FILENO, *most_recent_job) == -1)
		return -1;

	LinkedList *temp = get_command(jobs, *most_recent_job);
	print_list(temp);
	destroy_list(temp);
	
	if (killpg(*most_recent_job, SIGCONT) == -1)
		return -1;

	int status;

	if (waitpid(*most_recent_job, &status, WUNTRACED) == -1)
		return -1;

	if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1)
		return -1;

	if (WIFSTOPPED(status)) {
		printf("\nStopped: ");
		LinkedList *temp = get_command(jobs, *most_recent_job);
		print_list(temp);
		destroy_list(temp);
	} else {
		remove_pgid(jobs, *most_recent_job);
		*most_recent_job = 0;
	}

	return 0;
}

int move_to_background(ProcessMap *jobs, int *most_recent_job) {
	if (*most_recent_job == 0) {
		printf("No recent process to restart\n");
		return -1;
	}

	if (killpg(*most_recent_job, SIGCONT) == -1)
		return -1;

	printf("Running: ");
	LinkedList *temp = get_command(jobs, *most_recent_job);
	print_list(temp);
	destroy_list(temp);
	return 0;
}
