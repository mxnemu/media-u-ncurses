#include <curl/curl.h>
#include <ncurses.h>
#include <string.h>
#include "json.h"
#include "TvShows.h"
#include "CurlResult.h"
#include "Colors.h"
#include "EpisodeList.h"


void TvShows_init(struct TvShows* this) {
    this->lists = List_create();
    this->selectedList = NULL;
    this->scrollY = 0;
    int x, y;
    getmaxyx(stdscr, x,y);
    this->window = newwin(x,y, 0, 0);
    scrollok(this->window, true);
}

void TvShows_destroyMembers(struct TvShows* this) {
    List_destroyWithContent(this->lists, TvShowList);
}

DEFAULT_CREATE_DESTROY(TvShows)

void TvShows_handleInput(struct TvShows* this, int c) {
    if (c == 'n' || c == KEY_DOWN) {
	TvShows_selectDelta(this, +1);
    } else if (c == 'p' || c == KEY_UP) {
	TvShows_selectDelta(this, -1);
    } else if (c == '/') {
	// TODO search
    } else if (c == KEY_ENTER) {
	TvShows_playSelected(this);
    }
}

void TvShows_playSelected(struct TvShows* this) {
    struct TvShowLi* tvShow = TvShows_selectedTvShow(this);
    if (!tvShow) {
	return;
    }
    const char* name = tvShow->name;
    struct EpisodeList* episodeList = EpisodeList_fetch(this->baseUrl, name);
    EpisodeList_play(episodeList, this->baseUrl);
}

struct TvShowLi* TvShows_selectedTvShow(struct TvShows* this) {
    if (!this->selectedList) return NULL;
    struct TvShowList* list = this->selectedList->data;
    if (!list->selectedLi) return NULL;
    return list->selectedLi->data;
}

void TvShows_selectDelta(struct TvShows* this, int delta) {

    if (!this->selectedList) {
    	this->selectedList = this->lists->first;
    	delta = 1;
    	if (!this->selectedList) {
    	    return;
    	}
    }

    struct TvShowList* list = this->selectedList->data;
    bool inScope = TvShowList_selectDelta(list, this->window, delta);
    while (!inScope && this->selectedList) {
    	if (delta > 0) {
    	    this->selectedList = this->selectedList->next;
    	} else if (delta < 0) {
    	    this->selectedList = this->selectedList->previous;
    	}
    	if (this->selectedList) {
    	    list = this->selectedList->data;
    	    inScope = TvShowList_selectDelta(list, this->window, delta);
    	}
    }
}

int TvShows_printAll(struct TvShows* this) {
    int y = 0;
    for (ListNode* it = this->lists->first; it; it = it->next) {
	struct TvShowList* data = it->data;
	if (data) {
	    y = TvShowList_draw(data, this->window, y);
	}
    }
    if (this->window) {
        wrefresh(this->window);
    }
    return y;
}

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
    TvShows_recreateWindowForData(this);
    TvShows_printAll(this);
    this->baseUrl = baseUrl;
    json_value_free(json);
    CurlResult_destroyMembers(&userdata);
}

void TvShows_recreateWindowForData(struct TvShows* this) {
    if (this->window) {
        delwin(this->window);
    }
    this->window = NULL;
    int y = TvShows_printAll(this);
    int x = getmaxx(stdscr);
    this->window = newwin(y, x, 0, 0);
}

void TvShows_restore(struct TvShows* this, json_value* json) {
    if (!json || json->type != json_object) {
	printw("failed to parse json\n");
	refresh();
	return;
    }
    int length = json->u.object.length;
    for (int i=0; i < length; ++i) {
	char* itemName = json->u.object.values[i].name;
	if (0 == strcmp(itemName, "lists")) {
	    json_value* lists = json->u.object.values[i].value;
	    if (lists->type != json_object) {
		continue;
	    }
	    int listsLength = lists->u.object.length;
	    for (int j=0; j < listsLength; ++j) {
		char* name = lists->u.object.values[j].name;
		json_value* list = lists->u.object.values[j].value;
		TvShows_restoreList(this, name, list);
	    }
	}
    }
}

void TvShows_restoreList(struct TvShows* this, const char* name, json_value* json) {
    if (json->type != json_array) {
	return;
    }
    struct TvShowList* list = TvShowList_create();
    bool hasLis = false;
    int length = json->u.array.length;
    for (int i=0; i < length; ++i) {
	json_value* value = json->u.array.values[i];
	struct TvShowLi* li = TvShowLi_restore(value);
	if (li) {
	    hasLis = true;
	    List_pushBack(list->lis, li);
	}
    }
    if (hasLis) {
        size_t nameLength = strlen(name) + 1;
	list->name = malloc(sizeof(char) * nameLength);
	strcpy(list->name, name);
	List_qSort(list->lis, TvShowLi_compare);
	List_pushBack(this->lists, list);
    } else {
	free(list);
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


void TvShowList_init(struct TvShowList* this) {
    this->name = NULL;
    this->lis = List_create();
    this->selectedLi = NULL;
}

void TvShowList_destroyMembers(struct TvShowList* this) {
    List_destroyWithContent(this->lis, TvShowLi);
}

DEFAULT_CREATE_DESTROY(TvShowList)

int TvShowList_draw(struct TvShowList* this, WINDOW* window, int y) {
    if (y > 0) {
	++y;
    }
    if (window) {
        wmove(window, y, 0);
        wattron(window, A_UNDERLINE);
        wprintw(window, "list %s", this->name);
        wattroff(window, A_UNDERLINE);
    }
    ++y;
    for (ListNode* it = this->lis->first; it; it = it->next) {
	struct TvShowLi* data = it->data;
	if (data) {
            if (window) {
                wmove(window, y, 0);
                waddstr(window, data->name);
            }
            data->y = y;
	    ++y;
	}
    }
    return y;
}

bool TvShowList_selectDelta(struct TvShowList* this, WINDOW* window, int delta) {
    if (!this->selectedLi) {
	if (delta > 0) {
	    this->selectedLi = this->lis->first;
	} else if (delta < 0) {
	    this->selectedLi = this->lis->last;
	}

	if (this->selectedLi) {
	    struct TvShowLi* li = this->selectedLi->data;
	    TvShowLi_draw(li, window, true);
	    return true;
	}
	return false;
    }
    if (delta > 0) {
	struct TvShowLi* li = this->selectedLi->data;
	TvShowLi_draw(li, window, false);
	this->selectedLi = this->selectedLi->next;
    } else if (delta < 0) {
	struct TvShowLi* li = this->selectedLi->data;
	TvShowLi_draw(li, window, false);
	this->selectedLi = this->selectedLi->previous;
    } else {
	return false;
    }

    if (this->selectedLi) {
	struct TvShowLi* li = this->selectedLi->data;
	TvShowLi_draw(li, window, true);
    }

    return NULL != this->selectedLi;
}

void TvShowLi_init(struct TvShowLi* this) {
    this->name = NULL;
    this->watchedEpisodes = 0;
    this->totalEpisodes = 0;
    this->y = 0;
}

void TvShowLi_destroyMembers(struct TvShowLi* this) {
    if (this->name) {
	free(this->name);
    }
}

DEFAULT_CREATE_DESTROY(TvShowLi)

void TvShowLi_draw(struct TvShowLi* li, WINDOW* window, bool selected) {
    if (selected) {
	Colors_wset(window, Colors_Selected);
    }
    int x = getmaxx(stdscr);
    int clearChars = x - strlen(li->name);
    /* wscrl(window, -li->y); */
    /* wscrl(window, li->y); */
    wmove(window, li->y, 0);
    waddstr(window, li->name);
    for (int i=0; i < clearChars; ++i) {
	waddch(window, ' ');
    }
    if (selected) {
	Colors_wset(window, Colors_Default);
    }
    wmove(window, li->y, 0);
    wrefresh(window);
}

int TvShowLi_compare(void* av, void* bv) {
    struct TvShowLi* a = av;
    struct TvShowLi* b = bv;
    if (!a->name && !b->name) {
	return 0;
    }
    if (!a->name) {
	return -1;
    }
    if (!b->name) {
	return 1;
    }
    int result = strcmp(a->name, b->name);
    return result < 0 ? -1 :
	result > 0 ? 1 :
	0;
}
