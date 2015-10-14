#include <prodcons.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <future.h>

volatile int n;

sid32 produced, consumed;

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
                printf("Usage: %s [Count]\n\n", argv[0]);
                printf("Description:\n");
                printf("\tprints output of producer consumer processes upto count items. Assumes default of 2000.\n");
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
		future *f1, *f2, *f3;

		f1 = future_alloc(FUTURE_EXCLUSIVE);
		f2 = future_alloc(FUTURE_EXCLUSIVE);
		f3 = future_alloc(FUTURE_EXCLUSIVE);

		resume( create(future_cons, 1024, 20, "fcons1", 1, f1) );
		resume( create(future_prod, 1024, 20, "fprod1", 1, f1) );
		resume( create(future_cons, 1024, 20, "fcons2", 1, f2) );
		resume( create(future_prod, 1024, 20, "fprod2", 1, f2) );
		resume( create(future_cons, 1024, 20, "fcons3", 1, f3) );
		resume( create(future_prod, 1024, 20, "fprod3", 1, f3) );
	} else {
		pid32 producer_pid = create(producer, 1024, 20, "producer", 1, count);
		pid32 consumer_pid = create(consumer, 1024, 20, "consumer", 1, count);
		resume(producer_pid);
		resume(consumer_pid);
	}
	return 0;
}
