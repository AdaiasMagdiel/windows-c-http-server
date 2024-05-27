#ifndef MAG_SERVER
#define MAG_SERVER

#include <stdio.h>
#include <winsock2.h>
#include <string.h>

void initSocket();
void stopServer(SOCKET socketfd);
SOCKET createServer(char *host, int port);
SOCKET getConnection(SOCKET socketfd);
int sendData(SOCKET connectionfd, char *data);
char *notFound404();

#endif //MAG_SERVER
