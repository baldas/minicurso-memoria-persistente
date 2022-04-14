/*
 * https://gist.github.com/hilsonshrestha/8545379
 *
 * SINGLY LINKED LIST
 */

#include <stdio.h>
#include <stdlib.h>

/* necessary header file - PMDK */
#include <libpmemobj.h>

/*
 * The comments below the new sentences are the volatile equivalent.
 *
 */

struct Node {
	int data;
	PMEMoid p_next;
	//struct Node *next;
};


/* We usually put the root information inside a struct */
struct my_root {
  PMEMoid p_head;
};


typedef struct Node NODE;


/*
 * Read-only functions.
 */ 

void display(PMEMoid p_head) 
//void display(NODE *head) 
{
	PMEMoid p_current = p_head;
	//NODE *current = head;
	
  while (!OID_IS_NULL(p_current)) {
	//while (current != NULL) {
		printf(" %d ", ((NODE *)pmemobj_direct(p_current))->data);
		//printf(" %d ", current->data);
		
    p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
		//current = current->next;
	}

}


int searchByValue(PMEMoid p_head, int data) 
{
	PMEMoid p_current = p_head;
	//NODE *current = head;

	while (!OID_IS_NULL(p_current)) {
	//while (current != NULL) {
		if (((NODE *)pmemobj_direct(p_current))->data == data) 
		//if (current->data == data) 
      return 1;

    p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
    //current = current->next;
	}

  return 0;
}


/*
 * Modifying fuctions.
 */

/*
 * Must be called inside a transaction
 */
PMEMoid createNewNode(PMEMobjpool *pop, int data) 
//NODE *createNewNode(int data) {
{
  PMEMoid p_newNode;

  TX_BEGIN(pop) {

    p_newNode = pmemobj_tx_alloc(sizeof(NODE), 1);
	  //NODE *newNode = (NODE *) malloc(sizeof(NODE));
    
    ((NODE *)pmemobj_direct(p_newNode))->data = data;
	  //newNode->data = data;
    
    ((NODE *)pmemobj_direct(p_newNode))->p_next = OID_NULL;
	  //newNode->next = NULL;
	
  } TX_END

  return p_newNode;
//	return newNode;
}


void insertAtHead(PMEMobjpool *pop, PMEMoid *head, int data) 
//void insertAtHead(NODE **head, int data) {
{

  TX_BEGIN(pop) {

    PMEMoid p_newNode = createNewNode(pop, data);
	  //NODE *newNode = createNewNode(data);

    ((NODE *)pmemobj_direct(p_newNode))->p_next = *head;
	  //newNode->next = *head;

#ifndef NO_LOGGING
    pmemobj_tx_add_range_direct(head, sizeof(PMEMoid));
#endif

    *head = p_newNode;
	  //*head = newNode;

#ifdef SIMULATE_CRASH
    printf("Simulating a crash before the transaction commits but after the item is added to the head.\n");
    exit(1);
#endif

  } TX_END

}


void insertAtEnd(PMEMobjpool *pop, PMEMoid *head, int data) 
//void insertAtEnd(NODE **head, int data) {
{
  PMEMoid p_current = *head;
	//NODE *current = *head;

  TX_BEGIN(pop) {

    PMEMoid p_newNode = createNewNode(pop, data);
	  //NODE *newNode = createNewNode(data);

    if (OID_IS_NULL(p_current)) { // empty list
#ifndef NO_LOGGING
      pmemobj_tx_add_range_direct(head, sizeof(PMEMoid));
      //pmemobj_tx_add_range_direct(head, sizeof(struct my_root));
#endif
      *head = p_newNode;
    } else {

      while (!OID_IS_NULL(((NODE *)pmemobj_direct(p_current))->p_next))
        p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
		  //while (current->next != NULL) current = current->next;

#ifndef NO_LOGGING
      pmemobj_tx_add_range_direct(&((NODE *)pmemobj_direct(p_current))->p_next, sizeof(PMEMoid));
#endif
      ((NODE *)pmemobj_direct(p_current))->p_next = p_newNode;
		  //current->next = newNode;
    }

  } TX_END
}




void deleteByValue(PMEMobjpool *pop, PMEMoid *head, int data) 
//void deleteByValue(NODE **head, int data) {
{
  PMEMoid p_current = *head, p_previous;
	//NODE *current = *head, *previous;

  TX_BEGIN(pop) {

    while (!OID_IS_NULL(p_current)) {
	  //while (current != NULL) {

		  if (((NODE *)pmemobj_direct(p_current))->data == data) {
      //if (current->data == data) { // found

        if (OID_EQUALS(p_current, *head)) { // delete head
			  //if (current == *head) {
#ifndef NO_LOGGING
          pmemobj_tx_add_range_direct(head, sizeof(PMEMoid));
#endif
          *head = ((NODE *)pmemobj_direct(*head))->p_next;
				  //*head = (*head)->next;
          
          pmemobj_tx_free(p_current);
				  //free(current);
          
          p_current = *head;
				  //current = *head;
        } else {
#ifndef NO_LOGGING
          pmemobj_tx_add_range_direct(&(((NODE *)pmemobj_direct(p_previous))->p_next), sizeof(PMEMoid));
#endif
          ((NODE *)pmemobj_direct(p_previous))->p_next = ((NODE *)pmemobj_direct(p_current))->p_next;
				  //previous->next = current->next;
          
          pmemobj_tx_free(p_current);
				  //free(current);
          
          p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
				  //current = current->next;
        }

#ifdef SIMULATE_CRASH
        printf("Simulating a crash before the transaction commits but after the item is removed.\n");
        exit(1);
#endif

      } else {
        p_previous = p_current;
			  //previous = current;
        
        p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
			  //current = current->next;
      }
    }
  } TX_END

}




#define LAYOUT_NAME "linkedlist"

int main(int argc, char *argv[]) 
{

/****
 * POOL MANEGEMENT CODE
 */

/* Open the pool and return a "pool object pointer" */
  PMEMobjpool *pop = pmemobj_open(argv[1], LAYOUT_NAME);
	if (pop == NULL) {
    perror("pmemobj_open");
    return 1;
  }

/* Retrieve a persistent pointer to the root object */  
  PMEMoid p_root = pmemobj_root(pop, sizeof(struct my_root));

/* Get a "conventional" pointer to the root object */  
  struct my_root *root = pmemobj_direct(p_root);


	int option, data, location;
	while (1) {
		printf("\n\n SLL =");
		display(root->p_head);

		printf("\n\nEnter your choice:\n1. Insert at head\n2. Insert at end\n3. Delete by value\n4. Search by value\n5. Exit\n >> ");
		scanf("%d", &option);
		
		if (option == 1) {
			printf("Enter data to be inserted: ");
			scanf("%d", &data);
			insertAtHead(pop, &root->p_head, data);
		} else if (option == 2) {
			printf("Enter data to be inserted at end: ");
			scanf("%d", &data);
			insertAtEnd(pop, &root->p_head, data);
		} else if (option == 3) {
			printf("Enter value to be deleted: ");
			scanf("%d", &data);
			deleteByValue(pop, &root->p_head, data);
		} else if (option == 4) {
			printf("Enter value to be searched: ");
			scanf("%d", &data);
			if (searchByValue(root->p_head, data))
        printf("Item found!");
      else
        printf("Item NOT found!");
		} else if (option == 5) {
			break;
		}
	}

  pmemobj_close(pop);

  return 0;
  
}
