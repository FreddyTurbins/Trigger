#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Hello Triangle", 720, 460);
  while(!window_should_close()) {
    set_background(DARK_GRAY);
    
    draw_disk_thickness((Vector2){360.0f, 230.0f}, 200, 0.2f, (Color){0, 180, 90, 255});
    input_polling();
    gfx_update();
  }
  close_window();
  return 0;
}
