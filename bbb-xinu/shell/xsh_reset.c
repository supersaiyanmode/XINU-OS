#include <xinu.h>

shellcmd xsh_reset(int nargs, char *args[]) {
	volatile uint32 *prm_rstctrl = (volatile uint32*) 0x44E00F00;
	*prm_rstctrl = 0x01;
	return 0;
}
