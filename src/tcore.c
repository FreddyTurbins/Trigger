#include "TRIGGER/trigger.h"

#include <stdio.h>
#include <stdlib.h>


typedef struct { unsigned short width; unsigned short height; } Resolution;

typedef struct TriggerWindow {
  const char* title;
  bool vSync;
  bool fullscreen;
  bool shouldClose;
  Resolution screen;
  Resolution render;     // 
} TriggerWindow;

TriggerWindow triggerWindow = {0};

#if defined(PLATFORM_DESKTOP)
#include "platform/tcoregl.c"
#endif

void InitWindow(const char* title, unsigned short width, unsigned short height)
{
  if ((title != NULL) && (title[0] != 0)) triggerWindow.title = title;
  triggerWindow.render.width = width;
  triggerWindow.render.height = height;
  #if defined(PLATFORM_DESKTOP)
  InitOpenGL();
  #endif
}

bool WindowShouldClose(void)
{
  #if defined(PLATFORM_DESKTOP)
    return OpenGLShouldClose() || triggerWindow.shouldClose;
  #endif
  //TRACELOG WTF
  return true;
}

void WindowShutdown(void)
{
  triggerWindow.shouldClose = true;
}

void BeginDrawing(void)
{
}

void EndDrawing(void)
{
  #if defined(PLATFORM_DESKTOP)
  OpenGLSwapScreenBuffer();
  #endif
}

void CloseWindow(void)
{
  #if defined(PLATFORM_DESKTOP)
  OpenGLCloseWindow();
  #endif
}
