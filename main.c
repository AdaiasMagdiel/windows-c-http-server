/*
 * Copyright (C) 2025 Adaías Magdiel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <winsock2.h>

#define MGDL_SB_IMPLEMENTATION
#include "include/string_builder.h"

#define PORT 2002
#define MAX_CONNECTIONS 32
#define BUFFER_SIZE 4096

typedef struct Request {
  char *method;
  char *path;
} Request;

void parseRequest(Request *req, StringBuilder sb) {
  size_t lines_count;
  StringBuilder *lines = sb_split_to_builders(&sb, "\r\n", &lines_count);

  size_t met_path_count;
  StringBuilder *met_path =
      sb_split_to_builders(&lines[0], " ", &met_path_count);

  req->method = malloc(met_path[0].length + 1);
  strcpy(req->method, met_path[0].data);

  req->path = malloc(met_path[1].length + 1);
  strcpy(req->path, met_path[1].data);

  sb_free_array(met_path, met_path_count);
  sb_free_array(lines, lines_count);
}

void freeRequest(Request *req) {
  free(req->method);
  free(req->path);
}

int initializeWinsock() {
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    fprintf(stderr, "[ERROR] WSAStartup failed: %d\n", result);
    return -1;
  }
  return 0;
}

void cleanupWinsock() { WSACleanup(); }

SOCKET createSocket() {
  SOCKET serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverfd == INVALID_SOCKET) {
    fprintf(stderr, "[ERROR] Unable to create socket: %d\n", WSAGetLastError());
    return INVALID_SOCKET;
  }

  int enableReuseAddr = 1;
  if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                 (const char *)&enableReuseAddr,
                 sizeof(enableReuseAddr)) == SOCKET_ERROR) {
    fprintf(stderr, "[ERROR] Unable to set SO_REUSEADDR: %d\n",
            WSAGetLastError());
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
    fprintf(stderr, "[ERROR] Unable to bind: %d\n", WSAGetLastError());
    closesocket(serverfd);
    return SOCKET_ERROR;
  }

  result = listen(serverfd, MAX_CONNECTIONS);
  if (result == SOCKET_ERROR) {
    fprintf(stderr, "[ERROR] Unable to listen: %d\n", WSAGetLastError());
    closesocket(serverfd);
    return SOCKET_ERROR;
  }

  return 0;
}

SOCKET acceptConnection(SOCKET serverfd) {
  SOCKET client = accept(serverfd, NULL, NULL);
  if (client == INVALID_SOCKET) {
    fprintf(stderr, "[ERROR] Unable to accept client: %d\n", WSAGetLastError());
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

void sb_append_int(StringBuilder *sb, int value) {
  char buffer[32];
  snprintf(buffer, 32, "%d", value);

  sb_append(sb, buffer);
}

void readFile(Request req, StringBuilder *headers, StringBuilder *body) {
  char *file = req.path;
  if (file[0] == '/') {
    file++;
  }
  int res = sb_read_file(body, file);

  if (res == -1) {
    sb_reset(body);

    sb_append(body, "<h1 style=\"text-align: center;\">404 - Resource \"");
    sb_append(body, file);
    sb_append(body, "\" Not Found</h1>"
                    "<hr>"
                    "<p style=\"text-align: center;\">The page or resource you "
                    "are looking for could not be found.</p>");

    sb_append(headers, "HTTP/1.1 404 Not Found\r\n");
    sb_append(headers, "Content-Type: text/html\r\n");
    sb_append(headers, "Content-Length: ");
    sb_append_int(headers, body->length);

    return;
  }

  sb_append(headers, "HTTP/1.1 200 OK\r\n");
  sb_append(headers, "Content-Type: text/plain\r\n");
  sb_append(headers, "Content-Length: ");
  sb_append_int(headers, body->length);
}

void makeResponse(SOCKET client, Request req) {
  StringBuilder headers;
  sb_init(&headers);

  StringBuilder body;
  sb_init(&body);

  readFile(req, &headers, &body);

  char brk[] = "\r\n\r\n";
  int size = strlen(brk);

  send(client, headers.data, headers.length, 0);
  send(client, brk, size, 0);
  send(client, body.data, body.length, 0);

  sb_free(&headers);
  sb_free(&body);
}

void handleClient(SOCKET client) {
  StringBuilder sb;
  sb_init(&sb);
  sb_ensure_capacity(&sb, BUFFER_SIZE);

  int bytesReceived = recv(client, sb.data, BUFFER_SIZE, 0);
  if (bytesReceived > 0) {
    sb.data[bytesReceived] = '\0';

    Request req;
    parseRequest(&req, sb);

    printf("Method: %s | Path: %s\n", req.method, req.path);

    makeResponse(client, req);
    freeRequest(&req);
    sb_free(&sb);
  } else if (bytesReceived == 0) {
    printf("Client disconnected.\n");
  } else {
    fprintf(stderr, "[ERROR] recv failed: %d\n", WSAGetLastError());
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

  printf("Server listening on \x1b[32mhttp://localhost:%d\x1b[0m\n", PORT);

  while (1) {
    SOCKET client = acceptConnection(serverfd);
    if (client == INVALID_SOCKET) {
      closeServer(serverfd);
      cleanupWinsock();
      return 1;
    }

    handleClient(client);

    closeConnection(client);
  }

  closeServer(serverfd);
  cleanupWinsock();

  return 0;
}
