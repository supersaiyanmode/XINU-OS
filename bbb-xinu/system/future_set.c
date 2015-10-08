#include <future.h>
#include <stdio.h>

syscall future_set(future* f, int* value) {
	if (f == NULL) {
		return SYSERR;
	}
	
	if (f->state == FUTURE_VALID) {
		return SYSERR;
	}

	if (f->state == FUTURE_EMPTY || f->state == FUTURE_WAITING) {
		f->value = (int*)getmem(sizeof(int));
		*(f->value) = *value;
		printf("Setting value of: %d, %d\n", *(value), *(f->value));
		f->state = FUTURE_VALID;
		return OK;
	}
	return SYSERR;
}

