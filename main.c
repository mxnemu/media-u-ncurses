#include <curl/curl.h>
#include <ncurses.h>
#include "TvShows.h"
#include "Colors.h"

int main(int argc, const char* argv[]) {
    for (int i=0; i < argc; ++i) {
	printf("%s\n", argv[i]);
    }

    char* url = NULL;
    if (argc >= 2) {
	url = (char*)argv[1];
    } else {
	url = "localhost:8082";
    }

    curl_global_init(CURL_GLOBAL_SSL);
    initscr();
    if (!has_colors()) {
	puts("terminal doesn't support colors\n");
    }
    start_color();
    Colors_init();
    Colors_set(Colors_Default);

    // init ncurses io settings
    cbreak(); // allow ctrl-Z, ctrl-C and other default keys
    noecho();
    keypad(stdscr, TRUE);

    printw("%s fetching lists...\n", url);
    refresh();
    
    struct TvShows shows;
    TvShows_init(&shows);
    TvShows_fetch(&shows, url);
    refresh();
    
    bool quit = false;
    do {
	int c = getch();
	if (c == 27) {
	    quit = true;
	}
	TvShows_handleInput(&shows, c);
    } while (!quit);
    
    TvShows_destroyMembers(&shows);
    endwin();
    return 0;
}
