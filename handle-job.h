#ifndef HANDLE_JOB_H
#define HANDLE_JOB_H

int pipe_job (CommandBlock *first_command, CommandBlock *second_command, bool is_background, LinkedList* full_job_command, ProcessMap *processes, int *most_recent_job);

int job (CommandBlock *command, bool is_background, LinkedList* full_job_command, ProcessMap *processes, int *most_recent_job);

int monitor_background_jobs (ProcessMap *processes, int *most_recent_job);

int move_to_foreground(ProcessMap *jobs, int *most_recent_job);

int move_to_background(ProcessMap *jobs, int *most_recent_job);

#endif