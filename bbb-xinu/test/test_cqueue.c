#include <xinu.h>
#include <cqueue.h>

shellcmd test_cqueue(int argc, char** argv){
	int res = -1;
	QueueItem item;
	CQueue* queue = queue_alloc(5);

	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);

	queue_push(queue, (QueueItem){1});
	queue_walk(queue);
	queue_push(queue, (QueueItem){2});
	queue_walk(queue);
	queue_push(queue, (QueueItem){3});
	queue_push(queue, (QueueItem){4});
	queue_push(queue, (QueueItem){5});
	queue_walk(queue);

	printf("Trying to push extra element: %d\n", queue_push(queue, (QueueItem){6}));

	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);
	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);
	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);
	queue_walk(queue);
	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);
	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);
	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);
	res = queue_pop(queue, &item);
	printf("Trying to pop element: %d | ret=%d\n", item.value, res);
	printf("\n");
}
