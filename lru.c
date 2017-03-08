#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int time;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	int to_evict = 0;

	int least_recently_used = time;

	// find the frame with the earliest (least recent) access time
	int i;
	for (i=0; i< memsize; i++) {
		if(coremap[i].accessed_at < least_recently_used) {
			least_recently_used = coremap[i].accessed_at;
			to_evict = i;
		}
	}
	return to_evict;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	// update the last time the frame was accessed
	coremap[p->frame].accessed_at = time;

	// advance time
	time += 1;

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	time = 0;
	return;
}
