#ifndef MAG_REQUEST
#define MAG_REQUEST

#include <stdio.h>
#include <string.h>
#include <WinSock2.h>

#define RECV_BUFFER_LENGTH 512

typedef struct REQUEST {
	char method[32];
	char path[512];
} REQUEST;

REQUEST parseRecv(char *recvData);
REQUEST getRequest(SOCKET clientSocket);

#endif  // MAG_REQUEST
