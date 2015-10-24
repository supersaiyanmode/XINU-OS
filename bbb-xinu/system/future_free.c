#include <xinu.h>
#include <future.h>

syscall future_free(future* f) {
	if (f == NULL) {
		return SYSERR;
	}
	CQueue* q1 = f->set_queue;
	CQueue* q2 = f->get_queue;

	syscall v1 = queue_free(q1);
	syscall v2 = queue_free(q2);
	syscall v3 = OK;
	if (f->value) {
		v3 = freemem(f->value, sizeof(int));
		f->value = NULL;
	}
	syscall v4 = freemem((char*)f, sizeof(future));

	return v1 == SYSERR || v2 ==SYSERR || v3 ==SYSERR
		|| v4 == SYSERR? SYSERR: OK; 
}

