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

  //show text with highlighted search text
  portcolcon_set_color(con, PORTCOLCON_COLOR_WHITE, PORTCOLCON_COLOR_BLUE);
  portcolcon_write_highlight(con, "This a a text with highlighted search text.\n", "text", 0, PORTCOLCON_COLOR_YELLOW, PORTCOLCON_COLOR_RED);
  portcolcon_reset_color(con);

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
  portcolcon_write(con, "<-- Position [2,20]");

  //clean up
  portcolcon_cleanup(con);
  return 0;
}
