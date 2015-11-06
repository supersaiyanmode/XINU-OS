/* kill.c - kill */

#include <xinu.h>

int32 enable_kprintf_kill = -1;
#define STACK_INITIAL_BYTES 64

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	int32	i;			/* Index into descriptors	*/

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}

	char * prstktop;
	uint32 lenconsumed = prptr->prstklen - STACK_INITIAL_BYTES;
	prstktop = (char *)(prptr->prstkbase - prptr->prstklen - sizeof(uint32) + STACK_INITIAL_BYTES);
	
	while((*prstktop) == STACK_STUB_VALUE)
	{
		prstktop = prstktop + 1;
		lenconsumed--;
	}

		
	uint32 len = 256;
	kprintf("Here are the top 256 bytes in the stack while killing\n");
	prstktop = (char *)(prptr->prstkbase - prptr->prstklen + sizeof(uint32));
	int items_in_row = 32;
	for(; len>0; len--) {
		kprintf("%02x ", *prstktop++);
		if ((len-1)%items_in_row == 0)
			kprintf("\n");
	}
		
	kprintf("The total stack memory requested was %u and max consumed memory was %u\n", prptr->prstklen, lenconsumed); 
	
	freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}
