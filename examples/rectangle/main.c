#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Rectangle", 1920, 1080);
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    //Do draw Rectangle
    DrawRectangle((Vector2){-0.5f, -0.5f}, 1, 1, (Color){180, 30, 200, 255});
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
