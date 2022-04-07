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
 * The comments below the new sentences are the non-typed equivalent.
 *
 */


/*
 * Pool layout declaration
 *
 */
POBJ_LAYOUT_BEGIN(linkedlist);

POBJ_LAYOUT_ROOT(linkedlist, struct my_root);
POBJ_LAYOUT_TOID(linkedlist, struct Node);

POBJ_LAYOUT_END(linkedlist);



struct Node {
  int data;
  TOID(struct Node) p_next;
	//PMEMoid p_next;
};


/* We usually put the root information inside a struct */
struct my_root {
  TOID(struct Node) p_head;
  //PMEMoid p_head;
};


typedef struct Node NODE;


/*
 * Read-only functions.
 */ 
void display(TOID(struct Node) head) 
{
  TOID(struct Node) p_current = head;
	//PMEMoid p_current = p_head;
  
  while (!TOID_IS_NULL(p_current)) {
  //while (!OID_IS_NULL(p_current)) {
		printf(" %d ", D_RO(p_current)->data);
		//printf(" %d ", ((NODE *)pmemobj_direct(p_current))->data);

    p_current = D_RO(p_current)->p_next; 
    //p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
  }

}


int searchByValue(TOID(struct Node) head, int data) 
{
  TOID(struct Node) p_current = head;
	//PMEMoid p_current = p_head;

  while (!TOID_IS_NULL(p_current)) {
	//while (!OID_IS_NULL(p_current)) {
		if (D_RO(p_current)->data == data) 
		//if (((NODE *)pmemobj_direct(p_current))->data == data) 
      return 1;

    p_current = D_RW(p_current)->p_next;
    //p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
	}
  
  return 0;
}


/*
 * Modifying fuctions.
 */

/*
 * Must be called inside a transaction
 */
TOID(struct Node) createNewNode(PMEMobjpool *pop, int data) 
//PMEMoid createNewNode(PMEMobjpool *pop, int data) 
{
  TOID(struct Node) p_newNode;
  //PMEMoid p_newNode;

  TX_BEGIN(pop) {

    p_newNode = TX_NEW(struct Node);
    //p_newNode = pmemobj_tx_alloc(sizeof(NODE), 1);

    D_RW(p_newNode)->data = data;
    //((NODE *)pmemobj_direct(p_newNode))->data = data;
    
    D_RW(p_newNode)->p_next = TOID_NULL(struct Node);
    //((NODE *)pmemobj_direct(p_newNode))->p_next = OID_NULL;
	
  } TX_END

  return p_newNode;
}


void insertAtHead(PMEMobjpool *pop, TOID(struct Node) *head, int data) 
//void insertAtHead(PMEMobjpool *pop, PMEMoid *head, int data) 
{
  
  TX_BEGIN(pop) {

    TOID(struct Node) p_newNode = createNewNode(pop, data);
    //PMEMoid p_newNode = createNewNode(pop, data);

    D_RW(p_newNode)->p_next = *head;
    //((NODE *)pmemobj_direct(p_newNode))->p_next = *head;

#ifndef NO_LOGGING
    TX_ADD_DIRECT(head);
    //pmemobj_tx_add_range_direct(head, sizeof(PMEMoid));
#endif

    *head = p_newNode;

#ifdef SIMULATE_CRASH
    printf("Simulating a crash before the transaction commits but after the item is added to the head.\n");
    exit(1);
#endif

  } TX_END
  
}


void insertAtEnd(PMEMobjpool *pop, TOID(struct Node) *head, int data) 
//void insertAtEnd(PMEMobjpool *pop, PMEMoid *head, int data) 
{
  TOID(struct Node) p_current = *head;
  //PMEMoid p_current = *head;

  TX_BEGIN(pop) {

    TOID(struct Node) p_newNode = createNewNode(pop, data);
    //PMEMoid p_newNode = createNewNode(pop, data);

    if (TOID_IS_NULL(p_current)) { // empty list
    //if (OID_IS_NULL(p_current)) { // empty list
#ifndef NO_LOGGING
      TX_ADD_DIRECT(head);
      //pmemobj_tx_add_range_direct(head, sizeof(PMEMoid));
#endif
      *head = p_newNode;
    } else {

      while (!TOID_IS_NULL(D_RO(p_current)->p_next))
      //while (!OID_IS_NULL(((NODE *)pmemobj_direct(p_current))->p_next))
        p_current = D_RO(p_current)->p_next;
        //p_current = ((NODE *)pmemobj_direct(p_current))->p_next;

#ifndef NO_LOGGING
      TX_ADD_DIRECT(&D_RW(p_current)->p_next);
      //pmemobj_tx_add_range_direct(&((NODE *)pmemobj_direct(p_current))->p_next, sizeof(PMEMoid));
#endif

      D_RW(p_current)->p_next = p_newNode;
      //((NODE *)pmemobj_direct(p_current))->p_next = p_newNode;
    }

  } TX_END
  
}


void deleteByValue(PMEMobjpool *pop, TOID(struct Node) *head, int data) 
//void deleteByValue(PMEMobjpool *pop, PMEMoid *head, int data) 
{
  TOID(struct Node) p_current = *head, p_previous;
  //PMEMoid p_current = *head, previous;

  TX_BEGIN(pop) {

    while (!TOID_IS_NULL(p_current)) {
    //while (!OID_IS_NULL(p_current)) {

      if (D_RO(p_current)->data == data) { // found
		  //if (((NODE *)pmemobj_direct(p_current))->data == data) {

        if (TOID_EQUALS(p_current, *head)) { // delete head
        //if (OID_EQUALS(p_current, *head)) { // delete head
#ifndef NO_LOGGING
          TX_ADD_DIRECT(head);
          //pmemobj_tx_add_range_direct(head, sizeof(PMEMoid));
#endif
          *head = D_RO(*head)->p_next;
          //*head = ((NODE *)pmemobj_direct(*head))->p_next;
          
          TX_FREE(p_current);
          //pmemobj_tx_free(p_current);
          
          p_current = *head;
        } else {
#ifndef NO_LOGGING
          TX_ADD_DIRECT(&D_RW(p_previous)->p_next);
          //pmemobj_tx_add_range_direct(&(((NODE *)pmemobj_direct(p_previous))->p_next), sizeof(PMEMoid));
#endif
          D_RW(p_previous)->p_next = D_RO(p_current)->p_next;
          
          TX_FREE(p_current);
          //pmemobj_tx_free(p_current);
          
          p_current = D_RO(p_current)->p_next;
          //p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
        }

#ifdef SIMULATE_CRASH
        printf("Simulating a crash before the transaction commits but after the item is removed.\n");
        exit(1);
#endif

      } else {
        p_previous = p_current;
        
        p_current = D_RW(p_current)->p_next;
        //p_current = ((NODE *)pmemobj_direct(p_current))->p_next;
      }
    }
  } TX_END
  
}





int main(int argc, char *argv[]) 
{

/****
 * POOL MANEGEMENT CODE
 */

/* Open the pool and return a "pool object pointer" */
  PMEMobjpool *pop = pmemobj_open(argv[1], POBJ_LAYOUT_NAME(linkedlist));
  //PMEMobjpool *pop = pmemobj_open(argv[1], LAYOUT_NAME);
	if (pop == NULL) {
    perror("pmemobj_open");
    return 1;
  }

/* Retrieve a persistent pointer to the root object */  
  TOID(struct my_root) p_root = POBJ_ROOT(pop, struct my_root);
  //PMEMoid p_root = pmemobj_root(pop, sizeof(struct my_root));

/* Get a "conventional" pointer to the root object */  
  struct my_root *root = D_RW(p_root);
  //struct my_root *root = pmemobj_direct(p_root);


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
