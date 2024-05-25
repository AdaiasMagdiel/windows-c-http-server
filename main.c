#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "src/server.h"
#include "src/content.h"

#define RECV_BUFFER_LENGTH 512

typedef struct REQUEST {
	char method[32];
	char path[512];
} REQUEST;

REQUEST parseRecv(char *recvData) {
	char method[32];
	char path[512];

    int start;
	int end;
	int ptr;
    char c;

    // Get method
    start = 0;
	end = 0;
	ptr = 0;

	while (1) {
		c = recvData[ptr];

		if (c == ' ') {
			end = ptr;
            break;
		}

        ptr++;
	}

    for (int i=start; i<end; i++) {
        method[i] = recvData[i];
    }
    method[end] = '\0';

    // Get path
	start = 0;
	end = 0;
	ptr = 0;

	while (1) {
		c = recvData[ptr];

        if(c == '?' && start > 0) {
            end = ptr;
            break;
        }

		if (c == ' ') {
			if (start == 0) {
				start = ptr+1;
			} else {
                end = ptr;
                break;
            }
		}

        ptr++;
	}

    for (int i=start; i<end; i++) {
        path[i-start] = recvData[i];
    }
    path[end-start] = '\0';

    REQUEST req;
    strcpy(req.method, method);
    strcpy(req.path, path);
    
    return req;
}

int main() {
    char *htmlDir = "./public";

    char *hostAddress = "0.0.0.0";
    int portNumber = 3000;

    SOCKET serverSocket = createServer(hostAddress, portNumber);

    SOCKET clientSocket;
    while (1) {
    	char buff[RECV_BUFFER_LENGTH];

        clientSocket = getConnection(serverSocket);

        int res = recv(clientSocket, buff, RECV_BUFFER_LENGTH, 0);
		if (res == SOCKET_ERROR) {
			printf("Error: Unable to read recv.\n");
			break;
		}
		REQUEST req = parseRecv(buff);

		char htmlFilePath[512];
		strcpy(htmlFilePath, htmlDir);

		if (strcmp(req.path, "/") == 0) {
			strcat(htmlFilePath, "/index.html");
		} else {
			strcat(htmlFilePath, req.path);
		}


		// ----------------------------
		// printf("-----------\nBuff: \n%s\n-----------\n", buff);
		// printf("Method: >%s<\nPath: >%s<\n", req.method, req.path);
		// ----------------------------

		// req.method   ---   req.path
		printf("\x1b[37m%s\x1b[0m   ---   \x1b[37m%s\x1b[0m", req.method, req.path);

        char *htmlContent = getHTML(htmlFilePath);
        int bytes;
        if (htmlContent == NULL) {
        	// req.method   ---   req.path   ---   404
        	printf("   ---   \x1b[31m404\x1b[0m");

            bytes = sendData(clientSocket, notFound404());
        } else {
        	// req.method   ---   req.path   ---   200
	        printf("   ---   \x1b[32m200\x1b[0m");

	        bytes = sendData(clientSocket, htmlContent);

	        free(htmlContent);
        }

        // req.method   ---   req.path   ---   int:status   ---   Bytes: int
		printf("   ---   Bytes: %d\n", bytes);
    }
    closesocket(clientSocket);

    stopServer(serverSocket);
    return 0;
}
