#pragma once
#include "json.h"
#include "List.h"
#include "Constants.h"

struct TvShows {
    List* lists;
    ListNode* selectedList;
    int scrollY;
    WINDOW* window;
};

struct TvShowList {
    char* name;
    List* lis;
    ListNode* selectedLi;
};

struct TvShowLi {
    char* name;
    int watchedEpisodes;
    int totalEpisodes;
    int y;
};

void TvShows_init(struct TvShows* this);
void TvShows_destroyMembers(struct TvShows* this);
DEFAULT_CREATE_DESTROY_H(TvShow)

void TvShows_printAll(struct TvShows* this);
void TvShows_handleInput(struct TvShows* this, int c);
void TvShows_selectDelta(struct TvShows* this, int delta);

void TvShows_fetch(struct TvShows* this, const char* url);
void TvShows_restore(struct TvShows* this, json_value* json);
void TvShows_restoreList(struct TvShows* this, const char* name, json_value* json);

void TvShowList_init(struct TvShowList* this);
void TvShowList_destroyMembers(struct TvShowList* this);
DEFAULT_CREATE_DESTROY_H(TvShowList)

int TvShowList_draw(struct TvShowList* this, WINDOW* window, int y);
bool TvShowList_selectDelta(struct TvShowList* this, WINDOW* window, int delta);

void TvShowLi_init(struct TvShowLi* this);
void TvShowLi_destroyMembers(struct TvShowLi* this);
DEFAULT_CREATE_DESTROY_H(TvShowLi)

struct TvShowLi* TvShowLi_restore(json_value* json);
void TvShowLi_draw(struct TvShowLi* li, WINDOW* window, bool selected);
int TvShowLi_compare(void* a, void* b);
