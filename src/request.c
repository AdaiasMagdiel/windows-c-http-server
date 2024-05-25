#include "request.h"

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

REQUEST getRequest(SOCKET clientSocket) {
	char buff[RECV_BUFFER_LENGTH];

	int res = recv(clientSocket, buff, RECV_BUFFER_LENGTH, 0);
	if (res == SOCKET_ERROR) {
		printf("Error: Unable to read recv.\n");

		return (REQUEST) {0};
	}

	REQUEST req = parseRecv(buff);
	return req;
}
