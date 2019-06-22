#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* Tiny cmdline utility that sends a UDP broadcast
 * packet with the string 'time' as payload whenever
 * it is executed.
 *
 * (C) 2019 tobias.gierke@code-sourcery.de
 */
int main(int argc, char *argv[])
{
	int s;
	char* sMsg = "time";
	struct sockaddr_in dst;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	int bBroadcast = 1;
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&bBroadcast, sizeof(bBroadcast));
	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = INADDR_BROADCAST;
	dst.sin_port = htons(8000);
	sendto(s, sMsg, strlen(sMsg), 0, (struct sockaddr*) &dst, sizeof(dst));
	return 0;
}
