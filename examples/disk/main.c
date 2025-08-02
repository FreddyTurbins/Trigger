#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Hello Disk", 720, 460);
  init_renderer2d();
  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);
    draw_disk_thickness((Vector2){360.0f, 230.0f}, 200, 0.2f, (Color){0, 180, 90, 255});
    end_batch(); 
  }
  close_window();
  return 0;
}
