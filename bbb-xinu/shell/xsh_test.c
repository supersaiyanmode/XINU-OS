#include <xinu.h>
#include <test.h>

typedef shellcmd (*testfn)(int, char**);

typedef struct {
	shellcmd (*testfn)(int, char**);
	char name[50];
} testfn_t;

const testfn_t test_functions[] = {
	{test_cqueue, "cqueue"},
	{test_future, "future"},
};

shellcmd xsh_test(int argc, char * argv[]) {
	if (argc < 2) {
		printf("Requires atleast one parameter.\n");
		return -1;
	}
	char *command = argv[1];
	int i=0;
	for (; i<sizeof(test_functions)/sizeof(test_functions[0]); i++) {
		if (!strncmp(command, test_functions[i].name, strlen(command))) {
			return test_functions[i].testfn(argc-1, argv+1);
		}
	}
	return -1;
}
