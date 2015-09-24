#include <prodcons.h>

void producer(int count) {
	int i = 0;
	for (i=0; i<=count; i++) {
		n = i;
		kprintf("Produced: %d\n", i);
	}
}

