#pragma once
#include "Constants.h"
#include "List.h"
#include <stdbool.h>

struct EpisodeList {
    List* episodes;
};

#define EPISODE_NUMBER_INVALID -3
#define EPISODE_NUMBER_SPECIAL -2
#define EPISODE_NUMBER_UNKNOWN -1

struct Episode {
    int number;
    char* numberStr;
    char* path;
    bool watched;
};

struct _json_value;


void EpisodeList_init(struct EpisodeList* this);
void EpisodeList_destroyMembers(struct EpisodeList* this);
DEFAULT_CREATE_DESTROY_H(EpisodeList)

struct EpisodeList* EpisodeList_fetch(const char* baseurl, const char* tvShow);
struct EpisodeList* EpisodeList_restore(struct _json_value* json);

List* EpisodeList_contstructPlaylist(struct EpisodeList* this);
void EpisodeList_play(struct EpisodeList* this, const char* baseUrl);

void Episode_init(struct Episode* this);
void Episode_destroyMembers(struct Episode* this);
DEFAULT_CREATE_DESTROY_H(Episode)

struct Episode* Episode_restore(struct _json_value* episodeJson);
int Episode_compare(void* av, void* bv);
