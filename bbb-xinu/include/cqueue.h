#ifndef CQUEUE_H
#define CQUEUE_H

typedef struct CQueue CQueue;

typedef struct {
	int32 value;
} QueueItem;

CQueue* queue_alloc(int);

int queue_pop(CQueue*, QueueItem*);
int queue_push(CQueue*, QueueItem);
int queue_size(CQueue*);

void queue_walk(CQueue*);

syscall queue_free(CQueue*);

#endif
