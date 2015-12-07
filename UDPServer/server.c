#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<sys/types.h>

#define BUF_SIZE 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

int main(void)
{
	struct sockaddr_in sock_in, sock_remote;

	int s, i, recv_len;
	unsigned int slen = sizeof(sock_remote);
	char buf[BUF_SIZE];

	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		printf("Error in socket");
	}

	// zero out the structure
	memset((char *) &sock_in, 0, sizeof(sock_in));

	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(PORT);
	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if( bind(s , (struct sockaddr*)&sock_in, sizeof(sock_in) ) == -1)
	{
		printf("Error in bind");
	}

	printf("UDPServer has been started");
	//keep listening for data
	while(1)
	{
		printf("\nListening for data...");
		fflush(stdout);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr *) &sock_remote, &slen)) == -1)
		{
			printf("Error in recvfrom()");
		}

		//print details of the client/peer and the data received
		printf("\nPacket arrived from %s:%d\n", inet_ntoa(sock_remote.sin_addr), ntohs(sock_remote.sin_port));
		printf("Packet contents: %s\n" , buf);

		//now reply the client with the same data
		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &sock_remote, slen) == -1)
		{
			printf("Error in sendto()");
		}
	}

	close(s);
	return 0;
}

