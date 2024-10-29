#include "TRIGGER/trigger.h"

#include <stdio.h>
#include <stdlib.h>

unsigned int vao, vbo, ibo; 

typedef enum {
  NONE_API = 0,
  TEGL
} RendererApi;
static RendererApi currentRendererApi = NONE_API;

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
#include "tegl.h"               //THIS IS THE RIGHT ORDER
#include "platform/tecoregl.c"
#endif

void InitWindow(const char* title, const unsigned short width, const unsigned short height)
{
  if ((title != NULL) && (title[0] != 0)) triggerWindow.title = title;
  triggerWindow.render.width = width;
  triggerWindow.render.height = height;
  #if defined(PLATFORM_DESKTOP)
  InitOpenGL();
  tglInit();
  #endif
}

bool WindowShouldClose(void)
{
  #if defined(PLATFORM_DESKTOP)
  switch (currentRendererApi) {
    case NONE_API:
      TriggerLogCall(LOG_WARN, "WindowShouldClose function is not detecting an API");
      return true;
    case TEGL: return OpenGLShouldClose() || triggerWindow.shouldClose;
  }
  #endif
  TriggerLogCall(LOG_ERROR, "WindowShouldClose detecting an irregular rendererApi");
  return true;
}

void WindowShutdown(void)
{
  triggerWindow.shouldClose = true;
}
//Drawing functions
//============================================================
void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{
  tglSetUniform4(color.r, color.g, color.b, color.a);
  tglSetVertex3f(v1.x, v1.y, 0);
  tglSetVertex3f(v2.x, v2.y, 0);
  tglSetVertex3f(v2.x, v2.y, 0);
  tglSetVertex3f(v3.x, v3.y, 0);
}

void SetBackground(const Color color)
{
  switch (currentRendererApi) {
    case NONE_API: return;
    case TEGL: tglSetBackground(color.r, color.g, color.b, color.a); return;
  }
}

void CloseWindow(void)
{
  #if defined(PLATFORM_DESKTOP)
  OpenGLCloseWindow();
  #endif
}

void GFXUpdate(void) {
#if defined(PLATFORM_DESKTOP)
  OpenGLSwapScreenBuffer();
  tglClearScreenBuffer();
  tglDrawCurrentBatchRender();
#endif
}


void InputPolling(void) {

}
