#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "src/server.h"
#include "src/content.h"
#include "src/request.h"


int main() {
    char file[512] = "./public";

    char *hostAddress = "0.0.0.0";
    int portNumber = 3000;

    SOCKET serverSocket = createServer(hostAddress, portNumber);

    SOCKET clientSocket;
    while (1) {
        clientSocket = getConnection(serverSocket);

        REQUEST req = getRequest(clientSocket);

		if (strcmp(req.path, "/") == 0) {
			strcat(file, "/index.html");
		} else {
			strcat(file, req.path);
		}

		// req.method   ---   req.path
		printf("\x1b[37m%s\x1b[0m   ---   \x1b[37m%s\x1b[0m", req.method, req.path);

        char *content = getResponse(file);
        int bytes;
        if (content == NULL) {
        	// req.method   ---   req.path   ---   404
        	printf("   ---   \x1b[31m404\x1b[0m");

            bytes = sendData(clientSocket, notFound404());
        } else {
        	// req.method   ---   req.path   ---   200
	        printf("   ---   \x1b[32m200\x1b[0m");

	        bytes = sendData(clientSocket, content);

	        free(content);
        }

        // req.method   ---   req.path   ---   int:status   ---   Bytes: int
		printf("   ---   Bytes: %d\n", bytes);
    }
    closesocket(clientSocket);

    stopServer(serverSocket);
    return 0;
}
