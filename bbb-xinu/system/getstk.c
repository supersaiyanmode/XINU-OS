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

		/*if(++enable_kprintf_stack)
		  kprintf("Stacktop is %u\n", (uint32)stacktop);*/

		return (char *)memptr;
	}
	restore(mask);
	return (char *)SYSERR;
}
