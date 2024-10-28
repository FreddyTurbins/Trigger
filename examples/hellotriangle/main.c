#include <stdio.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"

signed main()
{
  InitWindow("Hello Triangle", 720, 460);
  ReadTextFile("");
  char* text = ReadTextFile("PEO.txt");
  printf("%s", text);
  free(text);
  while(!WindowShouldClose()) {
    SetBackground(DARK_GRAY);
    InputPolling();
    GFXUpdate();
  }
  CloseWindow();
  return 0;
}
