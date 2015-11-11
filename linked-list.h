#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct _ListNode {
	char* string;
	struct _ListNode *next;
} ListNode;

typedef struct {
	int length;
	int max_length;
	ListNode *head;
	ListNode *tail;
} LinkedList;

LinkedList *new_list(int max_length);
void destroy_list(LinkedList *list);
void push_back(LinkedList *list, char *string);
void push_front(LinkedList *list, char *string);
char *pop_front (LinkedList *list);
char *pop_back (LinkedList *list);
// void peek (LinkedList *list, char *string);
int get_length (LinkedList *list);
// void copy_data(char *from, void *to, int bytes);
LinkedList *copy_list(LinkedList *list_to_copy);
void print_list(LinkedList *list);


#endif
