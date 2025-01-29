#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Texture", 1920, 1080);
  Texture texture = CreateTexture("./test.png");
  Texture texture2 = CreateTexture("./test2.png");
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    
    DrawTexture(texture, (Vector2){300, 300}, (Color){255, 255, 255, 255});
    DrawTexture(texture2, (Vector2){1200, 300}, (Color){255, 0, 0, 255});
    
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
