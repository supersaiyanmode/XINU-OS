/* xsh_udpecho.c - xsh_udpecho */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

int test_suceeded = 0;
char	inbuf[1500];		/* buffer for incoming reply	*/

void receiver(char* msg, int len) {
	strncpy(inbuf, msg, len);
	kprintf("Message Received: %s\n", inbuf);
	test_suceeded = 1;
}

/*------------------------------------------------------------------------
 * xsh_udpecho - shell command that can send a message to a remote UDP
 *			echo server and receive a reply
 *------------------------------------------------------------------------
 */
shellcmd xsh_udpecho(int nargs, char *args[])
{
	int	i;			/* index into buffer		*/
	int	retval;			/* return value			*/
	char	msg[] = "Xinu testing UDP echo"; /* message to send	*/
	int32	slot;			/* UDP slot to use		*/
	int32	msglen;			/* length of outgoing message	*/
	uint32	remoteip;		/* remote IP address to use	*/
	//uint32	localip;		/* local IP address to use	*/
	uint16	echoport= 8888;		/* port number for UDP echo	*/
	uint16	locport	= 52743;	/* local port to use		*/
	int32	retries	= 3;		/* number of retries		*/
	int32	delay	= 2000;		/* reception delay in ms	*/

	/* For argument '--help', emit help about the 'udpecho' command	*/

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Use: %s  REMOTEIP [-f]\n\n", args[0]);
		printf("Description:\n");
		printf("\tBounce a message off a remote UDP echo server\n");
		printf("Options:\n");
		printf("\tREMOTEIP:\tIP address in dotted decimal\n");
		printf("\t-f:\tUse futures?\n");
		printf("\t--help\t display this help and exit\n");
		return 0;
	}

	/* Check for valid IP address argument */

	if (nargs != 2 && nargs != 3) {
		fprintf(stderr, "%s: invalid argument(s)\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
				args[0]);
		return 1;
	}

	if (dot2ip(args[1], &remoteip) == SYSERR) {
		fprintf(stderr, "%s: invalid IP address argument\r\n",
			args[0]);
		return 1;
	}
	/*
	localip = getlocalip();
	if (localip == SYSERR) {
		fprintf(stderr,
			"%s: could not obtain a local IP address\n",
			args[0]);
		return 1;
	}
	*/

	/* register local UDP port */
		
	int use_futures = 0;
	if (nargs >= 3 && !strncmp(args[2], "-f", 2)) 
		use_futures = 1;

	slot = udp_register(remoteip, echoport, locport);
	if (slot == SYSERR) {
		fprintf(stderr, "%s: could not reserve UDP port %d\n",
				args[0], locport);
		return 1;
	}
	kprintf("Slot is: %d\n", slot);

	if (use_futures) {
		udp_set_async(slot, receiver);
	}

	/* Retry sending outgoing datagram and getting response */

	msglen = strnlen(msg, 1200);
	for (i=0; i<retries; i++) {
		retval = udp_send(slot, msg, msglen);
		if (retval == SYSERR) {
			fprintf(stderr, "%s: error sending UDP \n",
				args[0]);
			return 1;
		}
		
		if (!use_futures) {
			retval = udp_recv(slot, inbuf, sizeof(inbuf), delay);
			if (retval == TIMEOUT) {
				fprintf(stderr, "%s: timeout...\n", args[0]);
				continue;
			} else if (retval == SYSERR) {
				fprintf(stderr, "%s: error from udp_recv \n",
					args[0]);
				udp_release(slot);
				return 1;
			}
			break;
		} else {
			sleepms(delay);
			if (test_suceeded) {
				retval = strnlen(inbuf, 1500);
				break;
			}
		}
	}
	
	udp_remove_async(slot);
	udp_release(slot);
	if (retval == TIMEOUT) {
		fprintf(stderr, "%s: retry limit exceeded\n",
			args[0]);
		return 1;
	}

	/* Response received - check contents */

	if (retval != msglen) {
		fprintf(stderr, "%s: sent %d bytes and received %d\n",
			args[0], msglen, retval);
		return 1;
	}
	for (i = 0; i < msglen; i++) {
		if (msg[i] != inbuf[i]) {
			fprintf(stderr, "%s: reply differs at byte %d\n",
				args[0], i);
			return 1;
		}
	}

	printf("UDP echo test was successful\n");
	return 0;
}
