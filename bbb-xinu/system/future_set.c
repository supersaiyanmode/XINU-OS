#include <xinu.h>
#include <future.h>
#include <stdio.h>

static syscall future_exclusive_set(future* f, int* value) {
	if (f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING) {
		f->value = (int*)getmem(sizeof(int));
		*(f->value) = *value;
		f->state = FUTURE_VALID;
		return OK;
	}
	return SYSERR;
}

static syscall future_shared_set(future* f, int* value) {
	f->value = (int*)getmem(sizeof(int));
	*(f->value) = *value;
	f->state = FUTURE_VALID;

	QueueItem proc;

	while (queue_pop(f->get_queue, &proc)) {
		printf("Resuming process: %d\n", proc.value);
		resume(proc.value);
	}
	return OK;
}

static syscall future_queue_set(future* f, int* value) {
	f->value = (int*)getmem(sizeof(int));
	*(f->value) = *value;
	f->state = FUTURE_VALID;
	
	QueueItem proc;
	if (queue_pop(f->get_queue, &proc)) {
		resume(proc.value);
	} else {
		pid32 pid = getpid();
		queue_push(f->set_queue, (QueueItem){pid});
		suspend(pid);
		if (queue_pop(f->get_queue, &proc)) {
			resume(proc.value);
		} else {
			return SYSERR;
		}
	}
	return OK;	
}

syscall future_set(future* f, int* value) {
	if (f == NULL) {
		return SYSERR;
	}
	
	int mask = disable();
	
	if (f->state == FUTURE_VALID) {
		restore(mask);
		return SYSERR;
	}

	syscall res = SYSERR;
	switch (f->flag) {
		case FUTURE_EXCLUSIVE:
			 res = future_exclusive_set(f, value);
			 break;
		case FUTURE_SHARED:
			 res = future_shared_set(f, value);
			 break;
		case FUTURE_QUEUE:
			 res = future_queue_set(f, value);
			 break;
	}
	restore(mask);
	return res;
}


