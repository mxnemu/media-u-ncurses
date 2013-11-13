#include "StringReplace.h"
#include "EpisodeList.h"
#include "CurlResult.h"
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>

void EpisodeList_init(struct EpisodeList* this) {
    this->episodes = List_create();
}
void EpisodeList_destroyMembers(struct EpisodeList* this) {
    List_destroyWithContent(this->episodes, Episode);
}
DEFAULT_CREATE_DESTROY(EpisodeList)

struct EpisodeList* EpisodeList_fetch(const char* baseurl, const char* tvShow) {
    char url[512];
    char* escapedTvShow = replaceStr(tvShow, " ", "%20");
    sprintf(url, "%s/api/library/tvShowDetails?%s", baseurl, escapedTvShow);

    CURL* handle = curl_easy_init();
    struct CurlResult userdata;
    CurlResult_init(&userdata);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult_write);
    curl_easy_perform(handle);
    json_value* json = CurlResult_parse(&userdata);
    CurlResult_destroyMembers(&userdata);
    
    struct EpisodeList* this = EpisodeList_restore(json);
    json_value_free(json);
    free(escapedTvShow);
    return this;
}

struct EpisodeList* EpisodeList_restore(json_value* json) {
    if (json->type != json_object) {
	return NULL;
    }

    struct EpisodeList* this = EpisodeList_create();
    for (int i=0; i < json->u.object.length; ++i) {
	if (0 == strcmp(json->u.object.values[i].name, "episodes")) {
	    struct Episode* ep = Episode_restore(json->u.object.values[i].value);
	    if (ep) {
		List_pushBack(this->episodes, ep);
	    }
	}
    }
    List_qSort(this->episodes, Episode_compare);
    return this;
}

List* EpisodeList_contstructPlaylist(struct EpisodeList* this) {
    List* list = List_create();
    for (ListNode* it = this->episodes->first; it; it = it->next) {
	struct Episode* ep = it->data;
	if (!ep->watched) {
	    List_pushBack(list, ep->path);
	}
    }
    if (!list->first) {
	List_destroy(list);
	return NULL;
    }
    return list;
}

void EpisodeList_play(struct EpisodeList* this, const char* baseUrl) {
    List* playList = EpisodeList_contstructPlaylist(this);

    if (!playList) {
	return;
    }
    /* TODO build json array
    char* jsonResponse;

    
    char url[1024*1024];
    sprintf(url, "%s/api/player/setPlayList?%s", baseUrl, jsonResponse);
    CURL* handle = curl_easy_init();
    struct CurlResult userdata;
    CurlResult_init(&userdata);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 15);
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &userdata);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, CurlResult_write);
    curl_easy_perform(handle);
    json_value* json = CurlResult_parse(&userdata);
    CurlResult_destroyMembers(&userdata);
    */
}

void Episode_init(struct Episode* this) {
    this->path = NULL;
    this->number = -3;
    this->numberStr = NULL;
    this->watched = false;
}

void Episode_destroyMembers(struct Episode* this) {
    if (this->path) {
	free(this->path);
    }
    if (this->numberStr) {
	free(this->numberStr);
    }
}
DEFAULT_CREATE_DESTROY(Episode)

struct Episode* Episode_restore(json_value* json) {
    if (json->type != json_object) {
	return NULL;
    }
    
    struct Episode* this = Episode_create();
    for (int i=0; i < json->u.object.length; ++i) {
	const char* key = json->u.object.values[i].name;
	json_value* value = json->u.object.values[i].value;
	if (0 == strcmp(key, "path") && value->type == json_string) {
	    size_t pathLength = value->u.string.length;
	    this->path = malloc(sizeof(char) * pathLength);
	    strcpy(this->path, value->u.string.ptr);
	} else if (0 == strcmp(key, "watched") && value->type == json_boolean) {
	    this->watched = value->u.boolean;
	}
    }
    return this;
}

int Episode_compare(void* av, void* bv) {
    struct Episode* a = av;
    struct Episode* b = bv;
    
    if (a->number < 0 && b->number >= 0) {
	return 1;
    }
    int cmpResult = strcmp(a->path, b->path);
    return cmpResult < 0 ? -1 :
	cmpResult > 0 ? 1 :
	0;
}
