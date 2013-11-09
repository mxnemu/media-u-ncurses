#pragma once
#include "json.h"

struct TvShows {
    int number;
};

struct TvShowLi {
    char* name;
    int watchedEpisodes;
    int totalEpisodes;
};

void TvShows_fetch(struct TvShows* this, const char* url);
void TvShows_restore(struct TvShows* this, json_value* json);
void TvShows_restoreList(struct TvShows* this, const char* name, json_value* json);

struct TvShowLi* TvShowLi_restore(json_value* json);
