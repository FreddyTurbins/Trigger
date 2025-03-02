#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

signed main()
{
  init_window("Shader example", 1920, 1080);
  init_renderer2d();
  unsigned int shader = load_shader("shader.vert", "shader.frag");
  Texture texture = load_texture("./test.png");
  Texture texture2 = load_texture("./test2.png");
  FontAtlas font = create_font_atlas("./font.bmp", (Vector2){16.0f, 5.0f});
  char buffer[32];

  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);

    begin_shader(shader); 
    {
      draw_texture(texture, (Vector2){300, 300}, WHITE);
      draw_texture(texture2, (Vector2){100, 100}, WHITE);
    }
    end_shader();

    draw_texture(texture2, (Vector2){1200, 300}, WHITE);
    draw_texture(texture2, (Vector2){1400, 300}, WHITE);
    draw_disk((Vector2){1700, 800}, 300, (Color){205, 25, 25, 255});
    snprintf(buffer, 32, "FPS: %4d", get_framerate());
    draw_rectangle_thickness((Rect){162, 1042, 310, 60}, 6.0f, WHITE);
    draw_text_atlas(font, buffer, (Vector2){20, 1000}, 4.0f, WHITE);
    draw_text_atlas(font, "Polifonia", (Vector2){500, 500}, 5.0f, WHITE);

    end_batch();
  }
  close_window();
  return 0;
}
