#include <stdio.h>
#include "src/server.h"
#include "src/content.h"

int main() {
    char *htmlFilePath = "./public/index.html";

    char *hostAddress = "0.0.0.0";
    int portNumber = 3000;

    SOCKET serverSocket = createServer(hostAddress, portNumber);

    SOCKET clientSocket;
    while (1) {
        clientSocket = getConnection(serverSocket);

        char *htmlContent = getHTML(htmlFilePath);
        if (htmlContent == NULL) {
            sendData(clientSocket, notFound404());
            continue;
        }

        sendData(clientSocket, htmlContent);
        free(htmlContent);
    }
    closesocket(clientSocket);

    stopServer(serverSocket);
    return 0;
}
