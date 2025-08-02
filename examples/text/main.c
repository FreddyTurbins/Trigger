#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Text example", 1920, 1080);
  init_renderer2d();
  FontAtlas texture = create_font_atlas("./font.bmp", (Vector2){8.0f, 5.0f});
  Texture texture2 = load_texture("./test.png");
  
  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);
    
    draw_texture(texture2, (Vector2){300, 300}, (Color){255, 255, 255, 255});
    draw_text_atlas(texture, "!#Hola/quetal?)", (Vector2){500, 300}, 10.0f, (Color){255, 255, 255, 255});

    input_polling();
    gfx_update();
    flush_renderer2d();
  }
  close_window();
  return 0;
}
