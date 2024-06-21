#include "TRIGGER/trigger.h"

#include <stdio.h>

int main(void)
{
  InitWindow("PICHULA", 640, 480);

  /* Loop until the user closes the window */
  while (!WindowShouldClose())
  {
    BeginDrawing();
    /* Render here */
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
