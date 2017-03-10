#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

extern struct frame *coremap;

// Contains address and pointer to next node
typedef struct linked_list_node {
  addr_t vaddr;
  struct linked_list_node* next;
} node;

// head of list
node* head;

// current trace
node* current;

// list of next reference distances
int* distance_to_reference;

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
  addr_t target;
  int i;

  // loop through everything
  for (i = 0; i < memsize; i++) {
    // looking for this vaddr in the following trace
    target = coremap[i].vaddr;

    // set default distance as 0
    int distance = 0;

    // search until found target or NULL. 
    // Increment distance while searching
    node* p = current->next;
    while (p && p->vaddr != target) {
      distance += 1;
      p = p->next;
    }

    // found frame
    if (p) {
      distance_to_reference[i] = distance;
    } 

    // didn't find frame
    else { 
      return i;
    }
  }

  int farthest_distance = -1;
  int fathest_frame = -1;

  // every frame is referenced later
  // find the longest distance that it is reference at
  for (i = 0; i < memsize; i++) {
    if (distance_to_reference[i] > farthest_distance) {
      fathest_frame = i;
      farthest_distance = distance_to_reference[i];
    }
  }
  return fathest_frame;

}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
  // Go to next node
  current = current->next;
  
  // If we hit the end, go back to the front
  if (!current) {
    current = head;
  }
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
  FILE* tfp = fopen(tracefile, "r");
  char buff[MAXLINE];
  addr_t vaddr;
  char type;

  // Initalize head and current
  head = NULL;
  current = NULL;

  // Go through the buffer to the end
  while (fgets(buff, MAXLINE, tfp) != NULL) {
    if (buff[0] != '=') {
      sscanf(buff, "%c %lx", &type, &vaddr);

      // create new node
      node* new_node = malloc(sizeof(node));
      new_node->vaddr = (vaddr >> PAGE_SHIFT) << PAGE_SHIFT;
      new_node->next = NULL;

      // add to the list
      if (head == NULL) {
        head = new_node;
        current = new_node;
      } 
      else {
        current->next = new_node;
        current = new_node;
      }

    }
  }

  // Set the current node to be the head
  current = head;

  // Allocate space for the list
  distance_to_reference = malloc(sizeof(int) * memsize);
}
