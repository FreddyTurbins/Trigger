#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Hello Rectangle", 1920, 1080);
  init_renderer2d();
  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);

    draw_quad((Rectangle){480.0f, 270.0f, 600.0f, 400.0f}, (Color){180, 30, 200, 255});
    input_polling();
    gfx_update();
    flush_renderer2d();
  }
  close_window();
  return 0;
}
