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
  portcolcon_write(con, "Color table:\nbg / fg\n");

  //show color table
  for (j = 0; j < 16; j++) {
    printf("%2i  ", j);
    for (i = 0; i < 16; i++) {
      portcolcon_set_color(con, i, j);
      printf(" %2i ", i);
    }
    portcolcon_reset_color(con);
    printf("\n");
  }

  //clean up
  portcolcon_cleanup(con);
}
