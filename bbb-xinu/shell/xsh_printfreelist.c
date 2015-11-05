#include <xinu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char *ptr;
	int len, used;
} Fragments;

Fragments fragments[15];

void allocmem() {
	int i = 0;
	for (; i<sizeof(fragments)/sizeof(fragments[0]); i++) {
		int len = rand() % 10000;
		fragments[i].ptr = getmem(len);
		fragments[i].len = len;
		fragments[i].used = 1;
		//printf("Got memory sized: %d at %x\n", fragments[i].len,
		//		fragments[i].ptr);
	}
	for (i=0; i<sizeof(fragments)/sizeof(fragments[0]); i++) {
		if (rand()%2) {
			freemem(fragments[i].ptr, fragments[i].len);
			fragments[i].used = 0;
			//printf("Freed memory sized: %d at %x\n",
			//	 fragments[i].len, fragments[i].ptr);
		}
	}
}

shellcmd xsh_printfree(int nargs, char *args[])
{
        if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
                printf("Usage: %s\n\n", args[0]);
                printf("Description:\n");
                printf("\tPrints a list of free memory blocks.\n");
                return 0;
        }

        /* Check argument count */

        if (nargs >= 2) {
                fprintf(stderr, "%s: too many arguments\n", args[0]);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        args[0]);
                return 1;
        }
	

	struct memblk *block;

	/* Output a heading for the free list */
	allocmem();
	printf("Free List:\n");
	printf("Block address  Length (dec)  Length (hex)\n");
	
	
	for (block = memlist.mnext; block != NULL; block = block->mnext) {
		printf("  0x%08x    %9d     0x%08x\n", block,
			block->mlength, block->mlength);
	}
	printf("\n");

	int i = 0;
	for (; i<sizeof(fragments)/sizeof(fragments[0]); i++) {
		if (fragments[i].used) {
			freemem(fragments[i].ptr, fragments[i].len);
			fragments[i].used = 0;
		}
	}
	return 0;
}

