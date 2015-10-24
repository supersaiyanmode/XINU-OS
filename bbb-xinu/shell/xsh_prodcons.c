#include <xinu.h>
#include <prodcons.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <future.h>

volatile int n;

sid32 produced, consumed;

uint32 future_prod_new1(future *fut) {
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

uint32 future_cons_new1(future *fut) {
	int i, status;
	status = future_get(fut, &i);
	if (status < 1) {
		printf("future_get failed\n");
		return -1;
	}
	printf("Future produced %d\n", i);
	return OK;
}

int parseNumber(char *str, int *ret, int max) {
	if (strnlen(str, max + 1) > max) {
		return 0;
	}
	int i=0, len=strnlen(str, max+1);
	int sum = 0;
	for (i=0; i<len; i++) {
		if (str[i] < '0' || str[i] > '9') {
			return 0;
		}
		sum = sum * 10 + (str[i]- '0');
	}
	*ret = sum;
	return 1;
}

shellcmd xsh_prodcons(int argc, char * argv[]) {
	if (argc == 2 && strncmp(argv[1], "--help", 7) == 0) {
		printf("Usage: %s [-f] [Count]\n\n", argv[0]);
		printf("Description:\n");
		printf("\tprints output of producer consumer processes upto count items. Assumes default of 2000.\n");
		printf("\t-f flag makes use of Futures. This is optional.\n");
		return 0;
	}

	if (argc > 3) {
		fprintf(stderr, "%s: Expects only one mandatory parameter and one optional parameter.\n", argv[0]);
		return PRODCONS_EXIT_BAD_ARGS;
	}
	int count = 2000;
	int useFuture = 0;
	if (argc == 2) {
		if (!strncmp(argv[1], "-f", 2)) {
			useFuture = 1;
		} else if (parseNumber(argv[1], &count, 9)) {
			//nothing. Number updated in the condition itself.
		} else {
			fprintf(stderr, "%s: Expected either a number or a '-f'\n", argv[0]);
			return PRODCONS_EXIT_BAD_COUNT;
		}
	} else if (argc == 3) {
		if (!parseNumber(argv[2], &count, 9)) {
			fprintf(stderr, "%s: Unable to parse number. It should not have any more than 9 numeric characters.\n", argv[0]);
			return PRODCONS_EXIT_BAD_COUNT;
		}
		if (strncmp(argv[1], "-f", 2)) {
			fprintf(stderr, "%s: Expected '-f' as the only optional parameter", argv[0]);
			return PRODCONS_EXIT_BAD_ARGS;
		} else {
			useFuture = 1;
		}
	}

	produced = semcreate(0);
	consumed = semcreate(1);

	if (useFuture == 1) {
		kprintf("Now starting the latest code\n");
		future *f_exclusive, *f_shared, *f_queue;

		f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
		f_shared = future_alloc(FUTURE_SHARED);
		f_queue = future_alloc(FUTURE_QUEUE);

		// Test FUTURE_EXCLUSIVE
		resume( create(future_cons_new1, 1024, 20, "fcons1", 1, f_exclusive) );
		resume( create(future_prod_new1, 1024, 20, "fprod1", 1, f_exclusive) );

		// Test FUTURE_SHARED
		resume( create(future_cons_new1, 1024, 20, "fcons2", 1, f_shared) );
		resume( create(future_cons_new1, 1024, 20, "fcons3", 1, f_shared) );
		resume( create(future_cons_new1, 1024, 20, "fcons4", 1, f_shared) ); 
		resume( create(future_cons_new1, 1024, 20, "fcons5", 1, f_shared) );
		resume( create(future_prod_new1, 1024, 20, "fprod2", 1, f_shared) );
		resume( create(future_cons_new1, 1024, 20, "fcons6", 1, f_shared) );
		resume( create(future_cons_new1, 1024, 20, "fcons7", 1, f_shared) );

		// Test FUTURE_QUEUE
		resume( create(future_cons_new1, 1024, 20, "fcons9", 1, f_queue) );
		resume( create(future_cons_new1, 1024, 20, "fcons10", 1, f_queue) );
		resume( create(future_cons_new1, 1024, 20, "fcons11", 1, f_queue) );
		resume( create(future_cons_new1, 1024, 20, "fcons12", 1, f_queue) );
		resume( create(future_prod_new1, 1024, 20, "fprod3", 1, f_queue) );

		resume( create(future_prod_new1, 1024, 20, "fprod4", 1, f_queue) );
		resume( create(future_prod_new1, 1024, 20, "fprod4", 1, f_queue) );


		resume( create(future_cons_new1, 1024, 20, "fcons5", 1, f_queue) );
		resume( create(future_prod_new1, 1024, 20, "fprod5", 1, f_queue) );
		resume( create(future_prod_new1, 1024, 20, "fprod6", 1, f_queue) );
	} else {
		pid32 producer_pid = create(producer, 1024, 20, "producer", 1, count);
		pid32 consumer_pid = create(consumer, 1024, 20, "consumer", 1, count);
		resume(producer_pid);
		resume(consumer_pid);
	}
	return 0;
}


