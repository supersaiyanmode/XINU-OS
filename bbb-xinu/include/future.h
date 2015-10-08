#ifndef __FUTURE__H__INCLUDED
#define __FUTURE__H__INCLUDED

#include <xinu.h>

#define FUTURE_EMPTY		0
#define FUTURE_WAITING		1
#define FUTURE_VALID		2

#define FUTURE_EXCLUSIVE	1

typedef struct futent {
	int *value;
	int flag;
	volatile int state;
	pid32 pid;
}future;

future* future_alloc(int);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);

#endif

