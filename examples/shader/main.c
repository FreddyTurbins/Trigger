#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Rectangle", 1920, 1080);
  unsigned int shader = LoadShader(NULL, "shader.frag");
  Texture texture = CreateTexture("./test.png");
  Texture texture2 = CreateTexture("./test2.png");
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);

    BeginShader(shader);
    DrawTexture(texture, (Vector2){300, 300}, (Color){255, 255, 255, 255});
    DrawTexture(texture2, (Vector2){100, 100}, (Color){255, 255, 255, 255});
    EndShader();
        
    DrawTexture(texture2, (Vector2){1200, 300}, (Color){255, 255, 255, 255});
    DrawTexture(texture2, (Vector2){1400, 300}, (Color){255, 255, 255, 255});
    printf("FPS: %d\n", GetFrameRate());

    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
