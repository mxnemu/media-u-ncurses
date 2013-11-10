#include "Colors.h"

void Colors_init() {
    init_pair(Colors_Default, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(Colors_Default));

    init_pair(Colors_Selected, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(Colors_Selected));
}
