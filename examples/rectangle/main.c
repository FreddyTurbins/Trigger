#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Rectangle", 1920, 1080);
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    //Do draw Rectangle
    DrawTriangle((Vector2){-0.5f, -0.5f}, (Vector2){0.5f, -0.5f}, (Vector2){0.5f, 0.5f}, (Color){0, 100, 120, 255});
    DrawTriangle((Vector2){0.5f, 0.5f}, (Vector2){-0.5f, 0.5f}, (Vector2){-0.5f, -0.5f}, (Color){0, 200, 120, 255});
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
