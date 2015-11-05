/* meminit.c - meminit */

#include <xinu.h>

void	*minheap;	/* Start address of heap	*/
void	*maxheap;	/* End address of heap		*/
void	*heaptop;
void	*stacktop;

/*------------------------------------------------------------------------
 * meminit - Initialize the free memory list for BeagleBone Black
 *------------------------------------------------------------------------
 */
void	meminit(void)
{
	struct	memblk *memptr;	/* Memory block pointer	*/

	/* Initialize the minheap and maxheap variables */

	minheap = (void *)&end;
	maxheap = (void *)MAXADDR;

	heaptop = (void *)minheap;
	stacktop = (void *)maxheap;
	

	/* Initialize the memory list as one big block */

	memlist.mnext = (struct memblk *)minheap;
	memptr = memlist.mnext;

	memptr->mnext = (struct memblk *)NULL;
	memptr->mlength = (uint32)maxheap - (uint32)minheap;
}
