#include <xinu.h>
#include <stdio.h>
#include <stddef.h>
#include <prodcons.h>
#include <stdlib.h>


int n;

int parseNumber(char *str, int *ret, int max) {
	*ret = atoi(str);
	return 1;
}

shellcmd xsh_prodcons(int argc, char * argv[]) {
	if (argc > 1 && argc != 2) {
		fprintf(stderr, "%s: Expects only one parameter.", argv[0]);
		return PRODCONS_EXIT_BAD_ARGS;
	}
	int count = 2000;
	if (argc == 2) {
		if (!parseNumber(argv[1], &count, 9)) {
			fprintf(stderr, "%s: Unable to parse number. It should not have any more than 9 numeric characters.", argv[0]);
			return PRODCONS_EXIT_BAD_COUNT;
		}
	}
	
	resume(create(producer, 1024, 20, "producer", 1, count));
	resume(create(consumer, 1024, 20, "consumer", 1, count));
	return 0;
}
