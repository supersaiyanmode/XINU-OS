#include <xinu.h>
#include <future.h>

future* future_alloc(int future_flag) {
	char* mem = getmem(sizeof(future));

	if (mem == SYSERR) {
		return SYSERR;
	}

	future *fut = (future*)mem;

	fut->value = NULL;
	fut->flag = future_flag;
	fut->state = FUTURE_EMPTY;
	fut->pid = NULLPROC;
	fut->set_queue = queue_alloc(FUTURE_PROCS_QUEUE_MAX);
	fut->get_queue = queue_alloc(FUTURE_PROCS_QUEUE_MAX);

	return fut;
}


