#include <xinu.h>
#include <prodcons.h>

void consumer(int count) {
	for (;;) {
		wait(produced);
		printf("Consumed: %d\n", n);
		//printf("Consumed Sem: %d\n", consumed);
		//printf("Produced Sem: %d\n", produced);

		if(n==count) {
			signal(consumed);
			break;
		}
			
		signal(consumed);
	}
}
