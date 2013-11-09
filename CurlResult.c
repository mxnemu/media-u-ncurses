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

void CurlResult_write(void* buffer, size_t bufferSize, void* userdata) {
    struct CurlResult* this = userdata;
    size_t newSize = this->bufferUsed + bufferSize;
    if (newSize > this->bufferAllocated) {
	size_t newAllocSize = this->bufferAllocated + bufferSize;
	char* newBuffer = malloc(newAllocSize);
	memcpy(newBuffer, this->buffer, this->bufferUsed);
	this->buffer = newBuffer;
	this->bufferAllocated = newAllocSize;
    }
    memcpy(this->buffer + this->bufferUsed, buffer, bufferSize);
}

json_value* CurlResult_parse(struct CurlResult* this) {
    return json_parse(this->buffer, this->bufferUsed);
}

DEFAULT_CREATE_DESTROY(CurlResult)
