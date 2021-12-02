#include "portcolcon.h"
#include <stdlib.h>

int main()
{
  portcolconhandle con;
  int i, j;

  //initialize
  con = portcolcon_initialize();

  //show text
  portcolcon_printf(con, "portcolcon v%s\n", portcolcon_get_version_string());

  //show console size
  if (portcolcon_get_size(con, &i, &j) != 0) {
    printf("Unable to get console dimensions\n");
  } else {
    printf("Console dimensions: [%i,%i]\n", i, j);
  }
  //draw rectangle around screen
  {
    int n;
    portcolcon_set_color(con, PORTCOLCON_COLOR_BLACK, PORTCOLCON_COLOR_WHITE);
    portcolcon_move_cursor(con, 0, j - 1);
    for (n = 0; n < i; n++) {
      portcolcon_write(con, "+");
    }
    portcolcon_move_cursor(con, 0, 0);
    for (n = 0; n < i; n++) {
      portcolcon_write(con, "+");
    }
    for (n = 1; n < j - 1; n++) {
      portcolcon_move_cursor(con, i - 1, n);
      portcolcon_write(con, "+");
      portcolcon_move_cursor(con, 0, n);
      portcolcon_write(con, "+");
    }
    portcolcon_reset_color(con);
  }
  portcolcon_move_cursor(con, 0, 3);

  //show text with highlighted search text
  portcolcon_set_color(con, PORTCOLCON_COLOR_WHITE, PORTCOLCON_COLOR_BLUE);
  portcolcon_write_highlight(con, "This a a text with highlighted search text.\n", "TEXT", 0, PORTCOLCON_COLOR_YELLOW, PORTCOLCON_COLOR_RED);
  portcolcon_set_color(con, PORTCOLCON_COLOR_WHITE, PORTCOLCON_COLOR_GREEN);
  portcolcon_write_multiple_highlights(con, "Some more text in which multiple search texts are being searched and highlighted.\n", (const char*[]){"TEXT", "search", "searched", "in", NULL}, 0, PORTCOLCON_COLOR_BRIGHT_MAGENTA, PORTCOLCON_COLOR_BROWN);
  portcolcon_reset_color(con);

  portcolcon_printf_in_color(con, PORTCOLCON_COLOR_GRAY, PORTCOLCON_COLOR_BLUE, "%.*s", 5, "Hello world!\n");
  portcolcon_printf(con, "\n");

  //wait for key while the cursor is hidden
  portcolcon_write(con, "Press Enter key to continue...");
  portcolcon_show_cursor(con, 0);
  getchar();
  portcolcon_show_cursor(con, 1);

  //clear screen
  portcolcon_clear_screen(con);

  //show color table
  portcolcon_write(con, "Color table:\nbg / fg\n");
  for (j = 0; j < 16; j++) {
    printf("%2i  ", j);
    for (i = 0; i < 16; i++) {
      portcolcon_set_color(con, i, j);
      printf(" %2i ", i);
    }
    portcolcon_reset_color(con);
    printf("\n");
  }

  //set the cursor
  portcolcon_move_cursor(con, 2, 20);
  portcolcon_write(con, "X <-- Position [2,20]\n");

  //clean up
  portcolcon_cleanup(con);
  return 0;
}
