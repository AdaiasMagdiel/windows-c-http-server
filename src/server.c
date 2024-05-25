#include "server.h"

void initSocket() {
	WSADATA wsa = {0};

	int result = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (result != 0) {
		printf("Error: Unable to start WSA.\n");
		exit(1);
	}
}

void stopServer(SOCKET socketfd) {
	closesocket(socketfd);
	WSACleanup();
}

SOCKET createServer(char *host, int port) {
	initSocket();

	int status = -1;
	struct sockaddr_in myaddr;

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(port);
	myaddr.sin_addr.s_addr = inet_addr(host);

	SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == INVALID_SOCKET) {
		printf("Error: Unable to create socket with error: %d\n", WSAGetLastError());

		closesocket(sockfd);
		WSACleanup();

		exit(1);
	}

	status = bind(sockfd, (SOCKADDR*) &myaddr, sizeof(myaddr));
	if (status == SOCKET_ERROR) {
		printf("Error: Unable to bind socket with error: %d\n", WSAGetLastError());

		closesocket(sockfd);
		WSACleanup();

		exit(1);
	}

	status = listen(sockfd, 2);
	if (status == SOCKET_ERROR) {
		printf("Error: Unable to listen for connections with error: %d\n", WSAGetLastError());

		closesocket(sockfd);
		WSACleanup();

		exit(1);
	}

	printf("\x1b[32m------------------------------------------\x1b[0m\n");
	printf("\x1b[32m| Server Running at:\x1b[0m \x1b[37mhttp://%s:%d\x1b[0m \x1b[32m|\x1b[0m\n", host, port);
	printf("\x1b[32m------------------------------------------\x1b[0m\n");

	return sockfd;
}

SOCKET getConnection(SOCKET socketfd) {
	SOCKET connectionfd = accept(socketfd, 0, 0);

	if (connectionfd == INVALID_SOCKET) {
		printf("Error: while get connection with error: %d\n", WSAGetLastError());

        closesocket(connectionfd);
        WSACleanup();

        exit(1);
	}

	return connectionfd;
}

int sendData(SOCKET connectionfd, char *data) {
	int bytes = send(connectionfd, data, (int)strlen(data), 0);
    if (bytes == SOCKET_ERROR) {
        printf("Error: Send failed with error: %d\n", WSAGetLastError());

        closesocket(connectionfd);
        WSACleanup();

        exit(1);
    }

    // shutdown the connection since no more data will be sent
    int shutdownRes = shutdown(connectionfd, SD_SEND);
    if (shutdownRes == SOCKET_ERROR) {
        wprintf(L"Error: Shutdown failed with error: %d\n", WSAGetLastError());

        closesocket(connectionfd);
        WSACleanup();

        exit(1);
    }

    return bytes;
}

char *notFound404() {
	return "HTTP/1.1 404 Not Found\r\nServer: Magotron2002\r\nContent-Type: text/html\r\n\r\n<h1 style=\"text-align: center; margin-block: 36px;\">404 Not Found</h1><hr><p style=\"text-align: center;\">Magotron2002</p>";
}
