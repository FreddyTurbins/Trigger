#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Triangle", 720, 460);
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    DrawCircle((Vector2){360.0f, 230.0f}, 100, (Color){0, 180, 90, 255});
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
