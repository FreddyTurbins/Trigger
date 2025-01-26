#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Triangle", 720, 460);
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    DrawTriangle((Vector2){-0.5f, -0.5f}, (Vector2){0.5f, -0.5f}, (Vector2){0.f, 0.5f}, (Color){0, 180, 90, 255});
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
