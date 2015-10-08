#include <prodcons.h>

void consumer(int count) {
	for (;;) {
		wait(produced);
		kprintf("Consumed: %d\n", n);

		if(n==count) {
			signal(consumed);
			break;
		}
			
		signal(consumed);
	}
	semdelete(produced);
	semdelete(consumed);
}
