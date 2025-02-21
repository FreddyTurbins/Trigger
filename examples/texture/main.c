#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Hello Texture", 1920, 1080);
  init_renderer2d();
  Texture texture = load_texture("./test.png");
  Texture texture2 = load_texture("./test3.png");
  SubTexture sub_texture = create_sub_texture(texture2, (Vector2){0, 0}, 
                                              (Vector2){texture2.width/4, texture2.height/5}, (Vector2){1.0f, 1.0f});
  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);
    
    draw_texture(texture, (Vector2){300, 300}, (Color){255, 255, 255, 255});
    draw_sub_texture_extended(sub_texture, (Rectangle){1200, 300, sub_texture.width, sub_texture.height}, 10.0f, (Color){255, 255, 255, 255});
    
    input_polling();
    gfx_update();
    flush_renderer2d();
  }
  close_window();
  return 0;
}
