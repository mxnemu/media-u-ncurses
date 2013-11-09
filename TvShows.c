#include <curl/curl.h>
#include <ncurses.h>
#include <string.h>
#include "json.h"
#include "TvShows.h"
#include "CurlResult.h"

void TvShows_fetch(struct TvShows* this, const char* baseUrl) {
    CURL* handle = curl_easy_init();
    struct CurlResult userdata;
    CurlResult_init(&userdata);
    char url[256];
    sprintf(url, "%s/%s", baseUrl, "api/library/filter/lists");
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult_write);
    curl_easy_perform(handle);
    json_value* json = CurlResult_parse(&userdata);
    TvShows_restore(this, json);
}

void TvShows_restore(struct TvShows* this, json_value* json) {
    if (json->type != json_object) {
	return;
    }
    int length = json->u.object.length;
    for (int i=0; i < length; ++i) {
	char* name = json->u.object.values[i].name;
	if (0 == strcmp(name, "lists")) {
	    json_value* list = json->u.object.values[i].value;
	    TvShows_restoreList(this, name, list);
	}
    }
}

void TvShows_restoreList(struct TvShows* this, const char* name, json_value* json) {
    if (json->type != json_array) {
	return;
    }
    int length = json->u.array.length;
    for (int i=0; i < length; ++i) {
	json_value* value = json->u.array.values[i];
	struct TvShowLi* li = TvShowLi_restore(value);
	if (li) {
	    free(li); // TODO insert into list
	}
    }
}

struct TvShowLi* TvShowLi_restore(json_value* json) {
    if (json->type != json_object) {
	return NULL;
    }
    struct TvShowLi* li = malloc(sizeof(struct TvShowLi));
    int showLength = json->u.object.length;
    for (int i=0; i < showLength; ++i) {
	char* key = json->u.object.values[i].name;
	json_value* value = json->u.object.values[i].value;
	if (0 == strcmp(key, "name") && value->type == json_string) {
	    int strLength = value->u.string.length;
	    li->name = malloc(sizeof(char)*strLength);
	    strcpy(li->name, value->u.string.ptr);
	} else if (0 == strcmp(key, "totalEpisodes") && value->type == json_integer) {
	    li->totalEpisodes = value->u.integer;
	} else if (0 == strcmp(key, "watchedEpisodes") && value->type == json_integer) {
	    li->watchedEpisodes = value->u.integer;
	}
    }
    return li;
}
