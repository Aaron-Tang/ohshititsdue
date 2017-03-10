#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_head;
/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	// Pointer for the current page table entry
	pgtbl_entry_t * current_entry;

	while (1){
		// Assigns value to current_entry
		current_entry = coremap[clock_head].pte;

		// Found, now break and return
		if ((current_entry->frame & PG_REF) == 0){
			break;
		}
		
		// Set to not used now and increment clock_head
		current_entry->frame &= ~PG_REF;
		clock_head += 1;
		
		// If we go pass, restart
		if (clock_head + 1 >= memsize){
			clock_head = 0;
		}
	}
	
	return clock_head;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	// Updates the page
	p->frame |= PG_REF;
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	// Initalize the clock_head as 0
	clock_head = 0;
}
