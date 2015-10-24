#ifndef __FUTURE__H__INCLUDED
#define __FUTURE__H__INCLUDED

#include <xinu.h>
#include <cqueue.h>

#define FUTURE_EMPTY		0
#define FUTURE_WAITING		1
#define FUTURE_VALID		2

#define FUTURE_EXCLUSIVE	1
#define FUTURE_SHARED		2
#define FUTURE_QUEUE		3

#define FUTURE_PROCS_QUEUE_MAX	15

typedef struct futent {
	int *value;
	int flag;
	volatile int state;
	pid32 pid;

	CQueue* set_queue;
	CQueue* get_queue;
}future;

future* future_alloc(int);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);

#endif

