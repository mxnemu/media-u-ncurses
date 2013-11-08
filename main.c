#include <ncurses.h>

int main(int argc, const char* argv[]) {
  for (int i=0; i < argc; ++i) {
    printf("%s\n", argv[i]);
  }
  
  initscr();
  printw("Hello World!");
  refresh();
  getch();
  endwin();

  return 0;
}
