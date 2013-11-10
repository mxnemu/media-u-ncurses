#pragma once
#include <curl/curl.h>
#include "json.h"
#include "Constants.h"

struct CurlResult {
    char* buffer;
    size_t bufferUsed;
    size_t bufferAllocated;
};

void CurlResult_init(struct CurlResult* this);
void CurlResult_destroyMembers(struct CurlResult* this);
size_t CurlResult_write(void* buffer, size_t charSize, size_t bufferSize, void* userdata);
json_value* CurlResult_parse(struct CurlResult* this);

DEFAULT_CREATE_DESTROY_H(CurlResult)
