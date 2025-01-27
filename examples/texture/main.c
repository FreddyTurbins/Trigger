#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Rectangle", 1920, 1080);
  Texture texture = CreateTexture("./test.png");
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    //Do draw Rectangle
    DrawTexture(texture, (Vector2){300, 300}, (Color){255, 255, 255, 255});
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
