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
    memcpy(this->buffer + this->bufferUsed, buffer, (bufferSize*nmemb));
    return bufferSize;
}

json_value* CurlResult_parse(struct CurlResult* this) {
    return json_parse(this->buffer, this->bufferUsed);
}

DEFAULT_CREATE_DESTROY(CurlResult)
