#include <xinu.h>
#include <prodcons.h>

void producer(int count) {
	int i = 0;
	for (i = 1; i<=count; i++) {
		wait(consumed);
		n = i;
		printf("Produced: %d\n", i);
		signal(produced);
	}
}

