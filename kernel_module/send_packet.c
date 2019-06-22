/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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
