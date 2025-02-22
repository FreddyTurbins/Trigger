#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Hello Rectangle", 1920, 1080);
  init_renderer2d();
  unsigned int shader = load_shader("shader.vert", "shader.frag");
  Texture texture = load_texture("./test.png");
  Texture texture2 = load_texture("./test2.png");
  FontAtlas font = create_font_atlas("./font.bmp", (Vector2){16.0f, 4.0f});
  char buffer[32];

  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);

    begin_shader(shader); 
    {
      draw_texture(texture, (Vector2){300, 300}, (Color){255, 255, 255, 255});
      draw_texture(texture2, (Vector2){100, 100}, (Color){255, 255, 255, 255});
    }
    end_shader();

    draw_texture(texture2, (Vector2){1200, 300}, (Color){255, 255, 255, 255});
    draw_texture(texture2, (Vector2){1400, 300}, (Color){255, 255, 255, 255});
    draw_disk((Vector2){1700, 800}, 300, (Color){205, 25, 25, 255});
    snprintf(buffer, 32, "FPS: %d", get_framerate());
    draw_rectangle_thickness((Rectangle){162, 1042, 310, 60}, 6.0f, (Color){255, 255, 255, 255});
    draw_text_atlas(font, buffer, (Vector2){20, 1000}, 4.0f, (Color){255, 255, 255, 255});
    draw_text_atlas(font, "Polifonia", (Vector2){500, 500}, 5.0f, (Color){255, 255, 255, 255});

    end_batch();
  }
  close_window();
  return 0;
}
