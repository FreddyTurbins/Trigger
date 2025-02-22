#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Hello Triangle", 720, 460);
  init_renderer2d();
  FontAtlas font = create_font_atlas("./font.bmp", (Vector2){16.0f, 4.0f});
  char buffer[32];
  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);
    
    draw_triangle((Vector2){180.0f, 115.0f}, (Vector2){540.0f, 115.0f}, (Vector2){360.0f, 345.0f}, (Color){0, 180, 90, 255});
    
    snprintf(buffer, 32, "FPS: %d", get_framerate());
    draw_text_atlas(font, buffer, (Vector2){0, 400}, 2.5f, (Color){255, 255, 255, 255});

    input_polling();
    gfx_update();
    flush_renderer2d();
  }
  close_window();
  return 0;
}
