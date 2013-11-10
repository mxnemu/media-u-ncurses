#include <string.h>
#include <ncurses.h>
#include <stdio.h>
#include "CurlResult.h"

void CurlResult_init(struct CurlResult* this) {
    this->buffer = malloc(256);
    this->bufferUsed = 0;
    this->bufferAllocated = 256;
}

void CurlResult_destroyMembers(struct CurlResult* this) {
    free(this->buffer);
}

size_t CurlResult_write(void* buffer, size_t charSize, size_t bufferSize, void* userdata) {
    if (!userdata) {
	return 0;
    }
    struct CurlResult* this = userdata;
    size_t newSize = this->bufferUsed + (bufferSize*charSize);
    if (newSize > this->bufferAllocated) {
	size_t newAllocSize = this->bufferAllocated + (bufferSize*charSize);
	char* newBuffer = malloc(newAllocSize);
	memcpy(newBuffer, this->buffer, this->bufferUsed);
	free(this->buffer);
	this->buffer = newBuffer;
	this->bufferAllocated = newAllocSize;
    }
    int offset = this->bufferUsed;
    offset = 0 == offset ? 0 : offset -1;
    memcpy(this->buffer + offset, buffer, (bufferSize*charSize));
    this->bufferUsed = this->bufferUsed + (bufferSize*charSize);
    return bufferSize;
}

json_value* CurlResult_parse(struct CurlResult* this) {
    FILE* f = fopen("jsonbuffer", "w");
    fwrite(this->buffer, sizeof(char), this->bufferUsed, f);
    char error[512];
    //json_settings settings;
    json_value* ret = json_parse_ex(NULL, this->buffer, this->bufferUsed, error);
    //    printw("error: %s\n", error);
    return ret;
}

DEFAULT_CREATE_DESTROY(CurlResult)
