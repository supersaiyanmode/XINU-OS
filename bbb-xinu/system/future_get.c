#include <future.h>

static syscall future_exclusive_get(future* f, int* value) {
	if (f->state == FUTURE_WAITING) {
		return SYSERR;
	}

	f->state = FUTURE_WAITING;
	f->pid = getpid();

	while (f->state != FUTURE_VALID);

	*value = *(f->value);
	f->state = FUTURE_EMPTY;
	f->pid = NULLPROC;
	return OK;
}

static syscall future_shared_get(future* f, int* value) {
	if (f->state == FUTURE_VALID) {
		*value = *(f->value);
		return OK;
	} else {
		pid32 pid = getpid();
		if (!queue_push(f->get_queue, (QueueItem){pid})) {
			return SYSERR;
		}
		suspend(pid);
		return OK;
	}
}

static syscall future_queue_get(future* f, int* value) {
	if (f->state == FUTURE_VALID) {
		*value = *(f->value);
		QueueItem proc;
		if (queue_pop(f->set_queue, &proc)) {
			resume(proc.value);
			return OK;
		} 
		return SYSERR;
	}

	//otherwise, suspend current thread
	pid32 pid = getpid();
	if (!queue_push(f->get_queue, (QueueItem){pid})) {
		return SYSERR;
	}
	suspend(pid);
	return OK;
}

syscall future_get(future* f, int* value) {
	if (f == NULL) {
		return SYSERR;	
	}

	int mask = disable();

	syscall res = SYSERR;
	switch(f->flag) {
		case FUTURE_EXCLUSIVE:
			res = future_exclusive_get(f, value);
			break;
		case FUTURE_SHARED:
			res = future_shared_get(f, value);
			break;
		case FUTURE_QUEUE:
			res = future_queue_get(f, value);
			break;
	}
	restore(mask);
	return res;
}


