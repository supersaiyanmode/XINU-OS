#include <future.h>

syscall future_free(future* f) {
	if (f == NULL) {
		return SYSERR;
	}
	
	return freemem((char*)f, sizeof(future)) == SYSERR? SYSERR: OK;
}

