#include "content.h"
#include <string.h>

// TODO: Add a log 

char* getFileContent(const char *filepath) {
    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL) {
        // fprintf(stderr, "Error: Unable to open file.\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fileSize < 0) {
        // fprintf(stderr, "Error: Unable to determine file size.\n");
        fclose(fp);
        return NULL;
    }

    size_t totalSize = (size_t)fileSize + 1;
    char *data = (char *)malloc(totalSize);
    if (data == NULL) {
        // fprintf(stderr, "Error: Unable to allocate memory.\n");
        fclose(fp);
        return NULL;
    }

    size_t bytesRead = fread(data, 1, (size_t)fileSize, fp);
    if (bytesRead != (size_t)fileSize) {
        // fprintf(stderr, "Error: Unable to read file content (read %zu bytes, expected %zu bytes).\n", bytesRead, (size_t)fileSize);
        free(data);
        fclose(fp);
        return NULL;
    }

    data[(size_t)fileSize] = '\0';

    fclose(fp);

    return data;
}

char* getHTTPHeader(const char *status, const char *contentType) {
    if (status == NULL) status = "200 OK";
    if (contentType == NULL) contentType = "text/html";

    size_t headerSize = strlen("HTTP/1.1 ") + strlen(status) + strlen("\r\n") +
                        strlen("Server: Magotron2002\r\n") +
                        strlen("Content-Type: ") + strlen(contentType) + strlen("\r\n\r\n") + 1;

    char *headerString = (char *)malloc(headerSize);
    if (headerString == NULL) {
        return NULL;
    }

    snprintf(headerString, headerSize, 
        "HTTP/1.1 %s\r\n"
        "Server: Magotron2002\r\n"
        "Content-Type: %s\r\n"
        "\r\n", 
    status, contentType);

    return headerString;
}

char* getFakeContentType(char *filepath, int is404) {
    int len = strlen(filepath)+1;
    int mark = 0;

    for (int i=0; i<len; i++) {
        if (filepath[i] == '.') mark = i;
    }

    char buffer[16];
    for (int i=mark; i<len; i++) {
        buffer[i-mark] = filepath[i];
    }
    buffer[len] = '\0';

    char *contentType = (char *)malloc(sizeof(char) * 48);
    if (contentType == NULL) return NULL;

    if (is404) {
        strcpy(contentType, "text/html");
        return contentType;
    }

    if (strcmp(buffer, ".html") == 0) {
        strcpy(contentType, "text/html");
    } else if (strcmp(buffer, ".css") == 0){
        strcpy(contentType, "text/css");
    } else if (strcmp(buffer, ".js") == 0){
        strcpy(contentType, "application/javascript");
    } else {
        strcpy(contentType, "text/plain");
    }

    return contentType;
}

RESPONSE getResponse(char *filepath) {
    char *status = "200 OK";
    int statusInt = 200;
    char *fileContent = getFileContent(filepath);

    char *contentType = getFakeContentType(filepath, fileContent == NULL);

    if (fileContent == NULL) {
        status = "404 Not Found";
        statusInt = 404;
        fileContent = "<h1 style=\"text-align: center; margin-block: 36px;\">404 Not Found</h1><hr><p style=\"text-align: center;\">Magotron2002</p>";
    }

    char *httpHeader =  getHTTPHeader(status, contentType);

    int len = strlen(httpHeader) + strlen(fileContent);

    char *res =  (char *)malloc(sizeof(char) * len + 1);
    strcpy(res, httpHeader);
    strcat(res, fileContent);

    free(contentType);
    free(httpHeader);

    RESPONSE response;
    response.status = statusInt;
    response.content = res;

    return response;
}
