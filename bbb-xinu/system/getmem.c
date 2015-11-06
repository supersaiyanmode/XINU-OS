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
		
		/*if(++enable_kprintf)
			kprintf("Heaptop is now %u\n", (uint32)heaptop);*/

		return (char *)memptr;
 	}

	restore(mask);
	return (char *)SYSERR;
}
