#define PRODCONS_EXIT_BAD_ARGS -1
#define PRODCONS_EXIT_BAD_COUNT -2

#include <future.h>

extern volatile int n;

extern sid32 produced, consumed;

void consumer(int);
void producer(int);

uint32 future_prod(future*);
uint32 future_cons(future*);
