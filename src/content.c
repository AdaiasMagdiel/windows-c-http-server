#include "content.h"

char* getHTML(const char *filepath) {
    const char *header = "HTTP/1.1 200 OK\r\nServer: Magotron2002\r\nContent-Type: text/html\r\n\r\n";
    size_t headerLen = strlen(header);

    FILE *fp = fopen(filepath, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Unable to open file.\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fileSize < 0) {
        fprintf(stderr, "Error: Unable to determine file size.\n");
        fclose(fp);
        return NULL;
    }

    size_t totalSize = headerLen + (size_t)fileSize + 1;
    char *data = (char *)malloc(totalSize);
    if (data == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory.\n");
        fclose(fp);
        return NULL;
    }

    strcpy(data, header);

    size_t bytesRead = fread(data + headerLen, 1, (size_t)fileSize, fp);
    if (bytesRead != (size_t)fileSize) {
        fprintf(stderr, "Error: Unable to read file content (read %zu bytes, expected %zu bytes).\n", bytesRead, (size_t)fileSize);
        free(data);
        fclose(fp);
        return NULL;
    }

    data[headerLen + (size_t)fileSize] = '\0';

    fclose(fp);

    return data;
}
