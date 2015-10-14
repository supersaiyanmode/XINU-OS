#include <future.h>

syscall future_get(future* f, int* value) {
	if (f == NULL) {
		return SYSERR;	
	}

	if (f->state == FUTURE_WAITING) {
		return SYSERR;
	}

	f->state = FUTURE_WAITING;
	f->pid = getpid();
	while (f->state != FUTURE_VALID);

	*value = *(f->value);

	if (freemem((char*)f->value, sizeof(int)) == SYSERR) {
		return SYSERR;
	}

	f->state = FUTURE_EMPTY;
	f->pid = NULLPROC;
	return OK;
}

