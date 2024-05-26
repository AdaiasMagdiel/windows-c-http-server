#ifndef MAG_CONTENT
#define MAG_CONTENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct RESPONSE {
	int status;
	char *content;
} RESPONSE;

char* getFileContent(const char *filepath);
char* getHTTPHeader(const char *status, const char *contentType);
char* getFakeContentType(char *filepath, int is404);
RESPONSE getResponse(char *filepath);

#endif  // MAG_CONTENT
