/* getmem.c - getmem */

#include <xinu.h>
uint32 enable_kprintf = -1;

/*------------------------------------------------------------------------
 *  getmem  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */

char  	*getmem(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	void * memptr;

	memptr = NULL;

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	if(((uint32)heaptop + nbytes) <= ((uint32)stacktop))	
	{
		memptr = heaptop;
		heaptop = (void *)((uint32)heaptop + nbytes);
			
		restore(mask);
		
		if(++enable_kprintf)
			kprintf("Heaptop is now %u\n", (uint32)heaptop);

		return (char *)memptr;
 	}
	else
	{
	}
		
	/**
	prev = &memlist;
	curr = memlist.mnext;
	while (curr != NULL) {				

		if (curr->mlength == nbytes) {		
			prev->mnext = curr->mnext;
			memlist.mlength -= nbytes;
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) {
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			memlist.mlength -= nbytes;
		
			restore(mask);
			return (char *)(curr);
		} else {			
			prev = curr;
			curr = curr->mnext;
		}
	}
	*/

	restore(mask);
	return (char *)SYSERR;
}
