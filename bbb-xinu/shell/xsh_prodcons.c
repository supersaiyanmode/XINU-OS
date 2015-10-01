#include <xinu.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <prodcons.h>

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

	if (argc > 1 && argc != 2) {
		fprintf(stderr, "%s: Expects only one parameter.\n", argv[0]);
		return PRODCONS_EXIT_BAD_ARGS;
	}
	int count = 2000;
	if (argc == 2) {
		if (!parseNumber(argv[1], &count, 9)) {
			fprintf(stderr, "%s: Unable to parse number. It should not have any more than 9 numeric characters.\n", argv[0]);
			return PRODCONS_EXIT_BAD_COUNT;
		}
	}

	produced = semcreate(0);
	consumed = semcreate(1);

	pid32 producer_pid = create(producer, 1024, 20, "producer", 1, count);
	pid32 consumer_pid = create(consumer, 1024, 20, "consumer", 1, count);
	resume(producer_pid);
	resume(consumer_pid);

	return 0;
}
