#include <future.h>

uint32 future_prod(future *fut) {
	int i, j = 0;
	//j = (int)fut;
	for (i=0; i<1000; i++) {
		j += i;
	}
	printf("Fina value: %d", j);
	future_set(fut, &j);
	return OK;
}
