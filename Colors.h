#include <ncurses.h>

void Colors_init();

#define Colors_Default 1
#define Colors_Selected 2
#define Colors_set(id)				\
    attron(COLOR_PAIR(id))
#define Colors_wset(window, id)			\
    wattron(window, COLOR_PAIR(id))
