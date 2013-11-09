#include <curl/curl.h>
#include <ncurses.h>
#include "json.h"
#include "TvShows.h"
#include "CurlResult.h"

void TvShows_fetch(struct TvShows* this, const char* baseUrl) {
    CURL* handle = curl_easy_init();
    struct CurlResult* userdata = CurlResult_create(&userdata);
    char url[256];
    sprintf(url, "%s/%s", baseUrl, "api/library/filter/lists");
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, userdata);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult_write);
    curl_easy_perform(handle);
    printw("done");
    puts(userdata->buffer);
    printw("size %d", userdata->bufferUsed);
    printw("Hello World! %s", userdata->buffer);
}
