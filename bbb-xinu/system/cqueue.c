#include <xinu.h>
#include <cqueue.h>

typedef struct CQueue {
	int size, max_size;
	int front, rear;
	int total_mem_size;
	QueueItem elements[];
} CQueue;

CQueue* queue_alloc(int size) {
	int total_size = size * sizeof (QueueItem) + sizeof(CQueue);
	CQueue *q = (CQueue*)getmem(total_size);
	if ((char*)q == (char*)SYSERR) {
		return (CQueue*)SYSERR;
	}
	q->front = q->rear = -1;
	q->size = 0;
	q->max_size = size;
	q->total_mem_size = total_size;

	return q;
}

syscall queue_free(CQueue* queue) {
	int size = queue->total_mem_size;
	return freemem((char*)queue, size);
}

int queue_push(CQueue* queue, QueueItem item) {
	if ((queue->front == 0 && queue->rear == queue->max_size -1)
			|| (queue->rear + 1 == queue->front)) {
		return 0;
	}
	if (queue->rear == queue->max_size - 1) {
		queue->rear = 0;
	} else {
		queue->rear++;
	}
	queue->elements[queue->rear] = item;
	if (queue->front == -1) {
		queue->front = 0;
	}
	return 1;
}

int queue_pop(CQueue* queue, QueueItem* item) {
	if (queue->front == -1) {
		return 0;
	}
	*item = queue->elements[queue->front];
	if (queue->front == queue->rear) {
		queue->front = queue->rear = -1;
	} else {
		if (queue->front == queue->max_size - 1)
			queue->front = 0;
		else
			queue->front++;
	}
	return 1;
}

void queue_walk(CQueue* queue) {
	if (queue->front == -1)
		return;
	int i=queue->front;
	for(; i != queue->rear; i = (i+1)%queue->max_size) {
		printf("%d ->", queue->elements[i]);
	}
	printf("%d\n", queue->elements[queue->rear]);
}
