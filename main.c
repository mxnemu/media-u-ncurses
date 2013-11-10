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
    Colors_set(Colors_Selected);

    printw("%s fetching lists...\n", url);
    refresh();
    
    struct TvShows shows;
    TvShows_fetch(&shows, url);

    int c = getch();
    putchar(c);
    endwin();

    return 0;
}
