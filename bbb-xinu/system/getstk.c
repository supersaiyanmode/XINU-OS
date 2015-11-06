/* getstk.c - getstk */

#include <xinu.h>

int32 enable_kprintf_stack = -1;

/*------------------------------------------------------------------------
 *  getstk  -  Allocate stack memory, returning highest word address
 *------------------------------------------------------------------------
 */
char  	*getstk(
		uint32	nbytes		/* Size of memory requested	*/
	       )
{
	intmask	mask;			/* Saved interrupt mask		*/

	void *memptr;
	uint32 count = 0;
	//struct	memblk	*prev, *curr;	/* Walk through memory list	*/
	//struct	memblk	*fits, *fitsprev; /* Record block that fits	*/

	memptr = NULL;
	mask = disable();

	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use mblock multiples	*/

	if(((uint32)stacktop - nbytes - sizeof(uint32)) >= (uint32)heaptop)
	{

		memptr = stacktop - sizeof(uint32);
		count = nbytes;

		/*
		if(++enable_kprintf_stack)
		{
			stacktop = (void *)(((uint32)stacktop) - sizeof(uint32));
			kprintf("count is : %u", count);
			while(count > 0)
			{
				*((byte *)stacktop) = STACK_STUB_VALUE;
				stacktop = (void *)(((uint32)stacktop) - 1);
				count--;
			} 
		}
		else
		{ */			
		stacktop = (void *)((uint32)stacktop - nbytes - sizeof(uint32));
		

		restore(mask);

		if(++enable_kprintf_stack)
		{
			kprintf("Stacktop is %u\n", (uint32)stacktop);
			kprintf("Stub value is %u\n", STACK_STUB_VALUE);
		}

		return (char *)memptr;
	}
	else
	{
	}
	/**
	  prev = &memlist;
	  curr = memlist.mnext;
	  fits = NULL;
	  fitsprev = NULL;  

	  while (curr != NULL) {			
	  if (curr->mlength >= nbytes) {	
	  fits = curr;		
	  fitsprev = prev;
	  }
	  prev = curr;
	  curr = curr->mnext;
	  }

	  if (fits == NULL) {			
	  restore(mask);
	  return (char *)SYSERR;
	  }
	  if (nbytes == fits->mlength) {		
	  fitsprev->mnext = fits->mnext;
	  } else {				
	  fits->mlength -= nbytes;
	  fits = (struct memblk *)((uint32)fits + fits->mlength);
	  }
	  memlist.mlength -= nbytes;

	 */

	restore(mask);
	return (char *)SYSERR;

	//return (char *)((uint32) fits + nbytes - sizeof(uint32));	
}
