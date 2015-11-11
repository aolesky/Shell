#include <stdlib.h>
#include <stdio.h>

#include "linked-list.h"
#include "process-map.h"

ProcessMap *new_map() {
	ProcessMap *map = malloc(sizeof(ProcessMap));
	map->size = 0;
	map->head = NULL;
	map->tail = NULL;

	return map;
}

void destroy_map(ProcessMap *map) {
	ProcessMapNode *current = map->head;
	while (current != NULL) {
		ProcessMapNode *next = current->next;
		destroy_list(current->command);
		free(current);
		current = next;
	}

	free(map);
}

int get_count(ProcessMap *map, int pgid) {
	ProcessMapNode *current = map->head;
	while (current != NULL) {
		if (current->pgid == pgid) {
			return current->count;
		}
		current = current->next;
	}
	return -1;
}

void add_process(ProcessMap *map, int pgid, LinkedList* command, int count) {
	ProcessMapNode *new_node = malloc(sizeof(ProcessMapNode));
	new_node->pgid = pgid;
	new_node->command = copy_list(command);
	new_node->next = NULL;
	new_node->count = count;

	map->size++;
	if (map->size == 1) {
		map->head = new_node;
		map->tail = new_node;
		return;
	}

	map->tail->next = new_node;
	map->tail = new_node;
}

LinkedList *get_command(ProcessMap *map, int pgid) {
	ProcessMapNode *current = map->head;
	while (current != NULL) {
		if (current->pgid == pgid) {
			return copy_list(current->command);
		}
		current = current->next;
	}

	return NULL;
}

int remove_pgid(ProcessMap *map, int pgid) {
	if (map->size == 0) {
		return -1;
	}

	ProcessMapNode *current = map->head;
	if (current->pgid == pgid) {
		ProcessMapNode *temp = current->next;
		if (map->size == 1) {
			map->head = NULL;
			map->tail = NULL;
		} else {
			map->head = temp;
		}
		destroy_list(current->command);
		free(current);
		map->size--;
		return 0;
	}

	while (current->next != NULL) {
		if (current->next->pgid == pgid) {
			ProcessMapNode *node_to_destroy = current->next;
			if (node_to_destroy == map->tail) {
				map->tail = current;
				current->next = NULL;
			} else {
				current->next = node_to_destroy->next;
			}
			destroy_list(node_to_destroy->command);
			free(node_to_destroy);
			map->size--;
			return 0;
		}
		current = current->next;
	}

	return -1;
}

int get_size (ProcessMap *map) {
	return map->size;
}

void print_processes(ProcessMap *map) {
	if (map->size == 0) {
		printf("There are no current processes.\n");
		return;
	}
	ProcessMapNode *current = map->head;

	while (current != NULL) {
		printf("%i: ", current->pgid);
		print_list(current->command);
		current = current->next;
	}
}

