/*
 * https://gist.github.com/hilsonshrestha/8545379
 *
 * SINGLY LINKED LIST
 */

#include <stdio.h>
#include <stdlib.h>

struct Node {
	int data;
	struct Node *next;
};

typedef struct Node NODE;

NODE *createNewNode(int data) {
	NODE *newNode = (NODE *) malloc(sizeof(NODE));
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}

void display(NODE *head) {
	NODE *current = head;
	while (current != NULL) {
		printf(" %d ", current->data);
		current = current->next;
	}
}


void insertAtHead(NODE **head, int data) {
	NODE *newNode = createNewNode(data);
	newNode->next = *head;
	*head = newNode;
}

void insertAtEnd(NODE **head, int data) {
	NODE *newNode = createNewNode(data), *current = *head;
	if (current == NULL) {
		*head = newNode;
	} else {
		// traverse until end of list is reached
		while (current->next != NULL) current = current->next;
		current->next = newNode;
	}
}

void deleteByValue(NODE **head, int data) {
	NODE *current = *head, *previous;
	while (current != NULL) {
		if (current->data == data) {
			if (current == *head) {
				*head = (*head)->next;
				free(current);
				current = *head;
			} else {
				previous->next = current->next;
				free(current);
				current = current->next;
			}
		} else {
			previous = current;
			current = current->next;
		}
	}
}

int searchByValue(NODE *head, int data) {
	NODE *current = head;
	while (current != NULL) {
		if (current->data == data) 
      return 1;

    current = current->next;
	}

	return 0; // not found
}




int main() {
	NODE *sll = NULL;
	int option, data, location;
	while (1) {
		printf("\n\n SLL =");
		display(sll);

		printf("\n\nEnter your choice:\n1. Insert at head\n2. Insert at end\n3. Delete by value\n4. Search by value\n5. Exit\n >> ");
		scanf("%d", &option);
		
		if (option == 1) {
			printf("Enter data to be inserted: ");
			scanf("%d", &data);
			insertAtHead(&sll, data);
		} else if (option == 2) {
			printf("Enter data to be inserted at end: ");
			scanf("%d", &data);
			insertAtEnd(&sll, data);
		} else if (option == 3) {
			printf("Enter value to be deleted: ");
			scanf("%d", &data);
			deleteByValue(&sll, data);
		} else if (option == 4) {
			printf("Enter value to be searched: ");
			scanf("%d", &data);
			if (searchByValue(sll, data))
        printf("Item found!");
      else
        printf("Item NOT found!");
		} else if (option == 5) {
			break;
		}
	}
	return 0;
}
