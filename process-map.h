#ifndef PROCESS_MAP_H
#define PROCESS_MAP_H

typedef struct _ProcessMapNode {
	int pgid;
	LinkedList* command;
	int count;
	struct _ProcessMapNode *next;
} ProcessMapNode;

typedef struct {
	int size;
	ProcessMapNode *head;
	ProcessMapNode *tail;
} ProcessMap;

ProcessMap *new_map();
void destroy_map(ProcessMap *map);
void add_process(ProcessMap *map, int pgid, LinkedList* command, int count);
LinkedList *get_command(ProcessMap *map, int pgid);
int remove_pgid(ProcessMap *map, int pid);
int get_size(ProcessMap *map);
void print_processes(ProcessMap *map);
int get_count(ProcessMap *map, int pgid);
//void change_count(ProcessMap *map, int gpid, int difference);

#endif
