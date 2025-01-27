#include "TRIGGER/trigger.h"

#include <stdio.h>
#include <stdlib.h>

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
  Resolution render;
} TriggerWindow;

TriggerWindow triggerWindow = {0};

#if defined(PLATFORM_DESKTOP)
  #define TEGL_IMPLEMENTATION
  #include "tgl.h"               //THIS IS THE RIGHT ORDER
  #include "platform/tcoregl.c"
#endif

static Texture texShapes = {1, 1, 1, 1, 7};
static Rectangle shapeRec = { 0.0f, 0.0f, 1.0f, 1.0f };

void InitWindow(const char* title, const unsigned short width, const unsigned short height)
{
  if ((title != NULL) && (title[0] != 0)) triggerWindow.title = title;
  triggerWindow.render.width = width;
  triggerWindow.render.height = height;
  #if defined(PLATFORM_DESKTOP)
  InitOpenGL();
  tglInit();
  currentRendererApi = TEGL;
  Mat4 projMatrix = CreateMatrixOrtho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
  tglSetUniformMat4f("projMatrix", projMatrix);
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
void DrawTriangle(const Vector2 v1, const Vector2 v2, const Vector2 v3, const Color color)
{
  tglSetUniform4(color.r, color.g, color.b, color.a);
  tglSetTexCoord2f(shapeRec.x/texShapes.width, shapeRec.y/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(v1.x, v1.y, 0);
  tglSetTexCoord2f(shapeRec.x/texShapes.width, (shapeRec.y + shapeRec.height)/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(v2.x, v2.y, 0);
  tglSetTexCoord2f((shapeRec.x + shapeRec.width)/texShapes.width, (shapeRec.y + shapeRec.height)/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(v2.x, v2.y, 0);
  tglSetTexCoord2f((shapeRec.x + shapeRec.width)/texShapes.width, shapeRec.y/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(v3.x, v3.y, 0);
  tglAddIndexCount();
}

void DrawRectangle(const Vector2 pos, const int width, const int height, const Color color)
{
  DrawRectangleExtended((Rectangle){pos.x, pos.y, width, height}, (Vector2){0.0f, 0.0f}, 0.0f, color);
}

void DrawRectangleExtended(const Rectangle data, const Vector2 origin, const float rotate, const Color color)
{
  Vector2 topLeft = { 0 };
  Vector2 topRight = { 0 };
  Vector2 bottomLeft = { 0 };
  Vector2 bottomRight = { 0 };
  if (rotate == 0.0f) {
    float x = data.x - origin.x;
    float y = data.y - origin.y;
    topLeft = (Vector2){x, y};
    topRight = (Vector2){x + data.width, y};
    bottomLeft = (Vector2){x, y + data.height};
    bottomRight = (Vector2){x + data.width, y + data.height};
  } else {
    TriggerLogCall(LOG_WARN, "ROTATE IS NOT IMPLEMENTED YET");
  }
  tglSetUniform4(color.r, color.g, color.b, color.a);
  tglSetTexCoord2f(shapeRec.x/texShapes.width, shapeRec.y/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(topLeft.x, topLeft.y, 0);
  tglSetTexCoord2f(shapeRec.x/texShapes.width, (shapeRec.y + shapeRec.height)/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(bottomLeft.x, bottomLeft.y, 0);
  tglSetTexCoord2f((shapeRec.x + shapeRec.width)/texShapes.width, (shapeRec.y + shapeRec.height)/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(bottomRight.x, bottomRight.y, 0);
  tglSetTexCoord2f((shapeRec.x + shapeRec.width)/texShapes.width, shapeRec.y/texShapes.height);
  tglSetTexIndex(0);
  tglSetVertex3f(topRight.x, topRight.y, 0);
  tglAddIndexCount();
}

void DrawTexture(const Texture texture, const Vector2 pos, const Color color)
{
  DrawTextureExtended(texture, (Rectangle){pos.x, pos.y, texture.width, texture.height}, 
      (Vector2){0.0f, 0.0f}, 0.0f, color);  
}

void DrawTextureExtended(const Texture texture, const Rectangle data, const Vector2 origin, const float rotate, const Color color)
{
  if (texture.id == 0) {
    TriggerLogCall(LOG_WARN, "TEXTURE-> Draw texture not valid");
    return;
  }
  Vector2 topLeft = {0};
  Vector2 topRight = {0};
  Vector2 bottomLeft = {0};
  Vector2 bottomRight = {0};
  if (rotate == 0.0f) {
    float x = data.x - origin.x;
    float y = data.y - origin.y;
    topLeft = (Vector2){x, y};
    topRight = (Vector2){x + data.width, y};
    bottomLeft = (Vector2){x, y + data.height};
    bottomRight = (Vector2){x + data.width, y + data.height};
  } else {
    TriggerLogCall(LOG_WARN, "ROTATE IS NOT IMPLEMENTED YET");
  }
  tglSetUniform4(color.r, color.g, color.b, color.a);
  tglSetTexCoord2f(0.0f, 1.0f);
  tglSetTexIndex(texture.id);
  tglSetVertex3f(topLeft.x, topLeft.y, 0);
  tglSetTexCoord2f(0.0f, 0.0f);
  tglSetTexIndex(texture.id);
  tglSetVertex3f(bottomLeft.x, bottomLeft.y, 0);
  tglSetTexCoord2f(1.0f, 0.0f);
  tglSetTexIndex(texture.id);
  tglSetVertex3f(bottomRight.x, bottomRight.y, 0);
  tglSetTexCoord2f(1.0f, 1.0f);
  tglSetTexIndex(texture.id);
  tglSetVertex3f(topRight.x, topRight.y, 0);
  tglAddIndexCount();
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

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

Texture CreateTexture(const char* filepath)
{
  Texture texture = {0};

  Image img = ReadImageFile(filepath);
  
  if (img.data == NULL) {
    TriggerLogCall(LOG_WARN, "TEXTURE -> [%s] Fail loading data from texture", filepath);
    return texture;
  }
  if (img.width == 0 || img.height == 0) {
    TriggerLogCall(LOG_WARN, "TEXTURE -> [%s] Dimension data is not valid for a texture", filepath);
    return texture;
  }
  
  //USING OPENGL ABSTRACTION
  texture.id = tglCreateTexture(img.data, img.width, img.height, img.nrChannel);
  texture.width = img.width;
  texture.height = img.height;
  texture.nrChannel = img.nrChannel;
  free(img.data); //DO ABSTRACTION
  if (texture.id == 0) {
    TriggerLogCall(LOG_WARN, "TEXTURE -> [%s] Fail creating texture", filepath);
  }

  return texture;
}

Image ReadImageFile(const char* filepath)
{
  Image img = {0};
  
  unsigned int dataCount = 0;
  unsigned char* imgData = (unsigned char*)ReadTextFile(filepath, &dataCount);

  if (imgData == NULL) {
    TriggerLogCall(LOG_WARN, "IMAGE -> [%s] Fail loading data from file", filepath);
    return img;
  }
  
  img.data = stbi_load_from_memory(imgData, dataCount, &img.width, &img.height, &img.nrChannel, 0);
  FreeTextData(imgData);

  if (img.data == NULL) {
    TriggerLogCall(LOG_WARN, "IMAGE -> [%s] Fail loading image from data", filepath);
  } else {
    TriggerLogCall(LOG_INFO, "IMAGE -> [%s -> %d/%d] Image loaded succesfuly", filepath, img.width, img.height);
  }

  return img;
}
