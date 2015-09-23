#include <prodcons.h>

void consumer(int count) {
	while (n <= count) {
		kprintf("Consumed: %d\n", n);
		if (n == count) {
			break;
		}
	}
}
