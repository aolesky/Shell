#include <stdlib.h>
#include <stdio.h>

#include "support-functions.h"
#include "linked-list.h"

LinkedList *new_list(int max_length) {
	LinkedList *list = malloc(sizeof(LinkedList));
	list->max_length = max_length;
	list->length = 0;
	list->head = NULL;
	list->tail = NULL;

	return list;
}

void destroy_list(LinkedList *list) {
	ListNode *current = list->head;
	while (current != NULL) {

		ListNode *temp = current->next;

		free(current->string);
		free(current);
		current = temp;
	}

	free(list);
}

void push_back(LinkedList *list, char *input_string) {
	ListNode *new_node = malloc(sizeof(ListNode));
	new_node->string = malloc(list->max_length);

	copy_string(input_string, new_node->string, list->max_length);

	list->length++;

	if (list->length == 1) {
		list->head = new_node;
		list->tail = new_node;
		new_node->next = NULL;
		return;
	}

	new_node->next = NULL;
	list->tail->next = new_node;
	list->tail = new_node;
}

void push_front(LinkedList *list, char *input_string) {
	ListNode *new_node = malloc(sizeof(ListNode));
	new_node->string = malloc(list->max_length);

	copy_string(input_string, new_node->string, list->max_length);

	list->length++;

	if (list->length == 1) {
		list->head = new_node;
		list->tail = new_node;
		new_node->next = NULL;
		return;
	}

	new_node->next = list->head;
	list->head = new_node;
}

char *pop_front (LinkedList *list) {
	if (get_length(list) == 0)
		return NULL;

	ListNode *node_to_remove = list->head;
	char *return_string = node_to_remove->string;
	list->head = node_to_remove->next;

	if (list->head == NULL)
		list->tail = NULL;

	free(node_to_remove);
	list->length--;

	return return_string;
}

char *pop_back (LinkedList *list) {
	if (get_length(list) == 0)
		return NULL;

	ListNode *node_to_remove = list->tail;
	char *return_string = node_to_remove->string;

	if (get_length(list) == 1) {
		list->head = NULL;
		list->tail = NULL;
		free(node_to_remove);
		list->length--;
		return return_string;
	}


	ListNode *current = list->head;
	while (current->next != node_to_remove && current->next != NULL) {
		current = current->next;
	}
	current->next = NULL;
	list->tail = current;
	
	free(node_to_remove);
	list->length--;

	return return_string;
}

// void peek (LinkedList *list, void *data) {
// 	copy_data(data, list->head->data, list->data_size);
// }

int get_length (LinkedList *list) {
	return list->length;
}

void print_list(LinkedList *list) {
	ListNode *current = list->head;

	if (current == NULL) {
		printf("List is empty\n");
		return;
	}

	while (current != NULL) {
		printf("%s ", current->string);
		current = current->next;
	}
	printf("\n");
}

LinkedList *copy_list(LinkedList *list_to_copy) {
	LinkedList *copy = new_list(list_to_copy->max_length);
	ListNode *current = list_to_copy->head;
	while (current != NULL) {
		push_back(copy, current->string);
		current = current->next;
	}
	return copy;
}

