#include <string.h>
#include "CurlResult.h"

void CurlResult_init(struct CurlResult* this) {
    this->buffer = malloc(256);
    this->bufferUsed = 0;
    this->bufferAllocated = 256;
}

void CurlResult_destroyMembers(struct CurlResult* this) {
    free(this->buffer);
}

size_t CurlResult_write(void* buffer, size_t bufferSize, size_t nmemb, void* userdata) {
    if (!userdata) {
	return 0;
    }
    struct CurlResult* this = userdata;
    size_t newSize = this->bufferUsed + (bufferSize*nmemb);
    if (newSize > this->bufferAllocated) {
	size_t newAllocSize = this->bufferAllocated + (bufferSize*nmemb);
	char* newBuffer = malloc(newAllocSize);
	memcpy(newBuffer, this->buffer, this->bufferUsed);
	free(this->buffer);
	this->buffer = newBuffer;
	this->bufferAllocated = newAllocSize;
    }
    int offset = this->bufferUsed;
    offset = 0 == offset ? 0 : offset -1;
    memcpy(this->buffer + offset, buffer, (bufferSize*nmemb));
    return bufferSize;
}

json_value* CurlResult_parse(struct CurlResult* this) {
    char error[512];
    //json_settings settings;
    json_value* ret = json_parse_ex(NULL, this->buffer, this->bufferUsed, error);
    printf("error: %s\n", error);
    return ret;
}

DEFAULT_CREATE_DESTROY(CurlResult)
