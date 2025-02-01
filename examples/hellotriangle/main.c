#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Triangle", 720, 460);
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    DrawTriangle((Vector2){180.0f, 115.0f}, (Vector2){540.0f, 115.0f}, (Vector2){360.0f, 345.0f}, (Color){0, 180, 90, 255});
    printf("FPS: %d\n", GetFrameRate());
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
