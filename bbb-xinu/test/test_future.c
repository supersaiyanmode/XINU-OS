#include <xinu.h>
#include <cqueue.h>
#include <future.h>

uint32 future_prod_new(future *fut) {
	static int val = 0;
	++val;
	int i, j = 0;
	for (i=0; i<1000 + val; i++) {
		j += i;
	}
	kprintf("Future Value has been set: %d\n", j);
	future_set(fut, &j);
	return OK;
}

uint32 future_cons_new(future *fut) {
	int i, status;
	status = future_get(fut, &i);
	if (status < 1) {
		printf("future_get failed\n");
		return -1;
	}
	printf("Future produced %d\n", i);
	return OK;
}

shellcmd test_future(int argc, char** argv){
	printf("Starting test_future\n");
	future *f_exclusive, *f_shared, *f_queue;

	f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
	f_shared = future_alloc(FUTURE_SHARED);
	f_queue = future_alloc(FUTURE_QUEUE);
#if 0
	// Test FUTURE_EXCLUSIVE
	resume( create(future_cons_new, 1024, 20, "fcons1", 1, f_exclusive) );
	resume( create(future_prod_new, 1024, 20, "fprod1", 1, f_exclusive) );
#endif

#if 0

	// Test FUTURE_SHARED
	resume( create(future_cons_new, 1024, 20, "fcons2", 1, f_shared) );
	resume( create(future_cons_new, 1024, 20, "fcons3", 1, f_shared) );
	resume( create(future_cons_new, 1024, 20, "fcons4", 1, f_shared) ); 
	resume( create(future_cons_new, 1024, 20, "fcons5", 1, f_shared) );
	resume( create(future_prod_new, 1024, 20, "fprod2", 1, f_shared) );
	resume( create(future_cons_new, 1024, 20, "fcons6", 1, f_shared) );
	resume( create(future_cons_new, 1024, 20, "fcons7", 1, f_shared) );
#endif

#if 1

	// Test FUTURE_QUEUE
	resume( create(future_cons_new, 1024, 20, "fcons9", 1, f_queue) );
	resume( create(future_cons_new, 1024, 20, "fcons10", 1, f_queue) );
	resume( create(future_cons_new, 1024, 20, "fcons11", 1, f_queue) );
	resume( create(future_cons_new, 1024, 20, "fcons12", 1, f_queue) );
	resume( create(future_prod_new, 1024, 20, "fprod3", 1, f_queue) );

	resume( create(future_prod_new, 1024, 20, "fprod4", 1, f_queue) );
	resume( create(future_prod_new, 1024, 20, "fprod4", 1, f_queue) );


	resume( create(future_cons_new, 1024, 20, "fcons5", 1, f_queue) );
	resume( create(future_prod_new, 1024, 20, "fprod5", 1, f_queue) );
	resume( create(future_prod_new, 1024, 20, "fprod6", 1, f_queue) );
#endif
}

