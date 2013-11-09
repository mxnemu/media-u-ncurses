#pragma once

struct TvShows {
    int number;
};

void TvShows_fetch(struct TvShows* this, const char* url);
