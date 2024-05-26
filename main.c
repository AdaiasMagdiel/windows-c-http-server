#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "src/server.h"
#include "src/content.h"
#include "src/request.h"


int main(int argc, char **argv) {

	char *publicDir;
	if (argc == 1) {
    	publicDir = "./public";
	} else {
		publicDir = argv[1];
	}

    char *hostAddress = "0.0.0.0";
    int portNumber = 3000;

    SOCKET serverSocket = createServer(hostAddress, portNumber);

    SOCKET clientSocket;
    while (1) {
        clientSocket = getConnection(serverSocket);

        REQUEST req = getRequest(clientSocket);

        char publicFile[512];
        strcpy(publicFile, publicDir);

		if (strcmp(req.path, "/") == 0) {
			strcat(publicFile, "/index.html");
		} else {
			strcat(publicFile, req.path);
		}

		// req.method   ---   req.path
		printf(
			"\x1b[37m%s\x1b[0m   ---   \x1b[37m%s\x1b[0m",
			req.method,                req.path
		);

        RESPONSE response = getResponse(publicFile);
        int bytes;

        bytes = sendData(clientSocket, response.content);

        // ---   Bytes: int
        printf("   ---   Bytes: %d", bytes);

        char *status = "\x1b[32m200\x1b[0m";
        if (response.status == 404) {
        	status = "\x1b[31m404\x1b[0m";
        }
        printf("   ---   %s\n", status);

        free(response.content);
    }
    closesocket(clientSocket);

    stopServer(serverSocket);
    return 0;
}
