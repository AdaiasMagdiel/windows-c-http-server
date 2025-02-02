#include <stdio.h>
#include <winsock2.h>

#define MGDL_SB_IMPLEMENTATION
#include "include/string_builder.h"

#define PORT 2002
#define MAX_CONNECTIONS 32
#define BUFFER_SIZE 4096

int initializeWinsock() {
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    printf("[ERROR] WSAStartup failed: %d\n", result);
    return -1;
  }
  return 0;
}

void cleanupWinsock() { WSACleanup(); }

SOCKET createSocket() {
  SOCKET serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverfd == INVALID_SOCKET) {
    printf("[ERROR] Unable to create socket: %d\n", WSAGetLastError());
    return INVALID_SOCKET;
  }

  int enableReuseAddr = 1;
  if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                 (const char *)&enableReuseAddr,
                 sizeof(enableReuseAddr)) == SOCKET_ERROR) {
    printf("[ERROR] Unable to set SO_REUSEADDR: %d\n", WSAGetLastError());
    closesocket(serverfd);
    return INVALID_SOCKET;
  }

  return serverfd;
}

struct sockaddr_in createAddress() {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);
  return addr;
}

int startServer(SOCKET serverfd, struct sockaddr_in addr) {
  int result = bind(serverfd, (SOCKADDR *)&addr, sizeof(addr));
  if (result == SOCKET_ERROR) {
    printf("[ERROR] Unable to bind: %d\n", WSAGetLastError());
    closesocket(serverfd);
    return SOCKET_ERROR;
  }

  result = listen(serverfd, MAX_CONNECTIONS);
  if (result == SOCKET_ERROR) {
    printf("[ERROR] Unable to listen: %d\n", WSAGetLastError());
    closesocket(serverfd);
    return SOCKET_ERROR;
  }

  return 0;
}

SOCKET acceptConnection(SOCKET serverfd) {
  SOCKET client = accept(serverfd, NULL, NULL);
  if (client == INVALID_SOCKET) {
    printf("[ERROR] Unable to accept client: %d\n", WSAGetLastError());
    return INVALID_SOCKET;
  }
  return client;
}

void closeConnection(SOCKET client) {
  shutdown(client, SD_SEND);
  closesocket(client);
}

void closeServer(SOCKET serverfd) {
  shutdown(serverfd, SD_BOTH);
  closesocket(serverfd);
}

void proccessResponse(StringBuilder *sb) {
  printf("[Request]:\n%s\n", sb->data);

  sb_free(sb);
  sb_init(sb);

  sb_append(sb, "HTTP/1.1 200 OK\r\n");
  sb_append(sb, "Content-Type: text/plain\r\n");
  sb_append(sb, "\r\n");
  sb_append(sb, "pong\r\n");
}

void handleClient(SOCKET client) {
  StringBuilder sb;
  sb_init(&sb);
  sb_ensure_capacity(&sb, BUFFER_SIZE);

  int bytesReceived = recv(client, sb.data, BUFFER_SIZE, 0);
  if (bytesReceived > 0) {
    sb.data[bytesReceived] = '\0';

    proccessResponse(&sb);

    send(client, sb.data, sb.length, 0);
    sb_free(&sb);
  } else if (bytesReceived == 0) {
    printf("Client disconnected.\n");
  } else {
    printf("[ERROR] recv failed: %d\n", WSAGetLastError());
  }
}

int main() {
  if (initializeWinsock() != 0) {
    return 1;
  }

  SOCKET serverfd = createSocket();
  if (serverfd == INVALID_SOCKET) {
    cleanupWinsock();
    return 1;
  }

  struct sockaddr_in addr = createAddress();
  if (startServer(serverfd, addr) != 0) {
    closeServer(serverfd);
    cleanupWinsock();
    return 1;
  }

  printf("Server listening on http://localhost:%d...\n", PORT);

  while (1) {
    SOCKET client = acceptConnection(serverfd);
    if (client == INVALID_SOCKET) {
      closeServer(serverfd);
      cleanupWinsock();
      return 1;
    }

    printf("Client connected.\n");
    handleClient(client);

    closeConnection(client);
  }

  closeServer(serverfd);
  cleanupWinsock();

  return 0;
}
