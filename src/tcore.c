#include "TRIGGER/trigger.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum {
  NONE_API = 0,
  TEGL
} RendererApi;
static RendererApi currentRendererApi = NONE_API;

typedef struct {unsigned short width; unsigned short height;} Resolution;

typedef struct TriggerWindow {
  const char* title;
  bool vSync;
  bool fullscreen;
  bool shouldClose;
  Resolution screen;
  Resolution render;
} TriggerWindow;

TriggerWindow triggerWindow = {0};

#define COLOR_NUMBER(X)                    ((X).r<<(8*3))+((X).g<<(8*2))+((X).b<<(8*1))+(X).a

#if defined(PLATFORM_DESKTOP)
  #define TMATH_IMPLEMENTATION
  #include "tmath.h"
  #define TEGL_IMPLEMENTATION
  #include "tgl.h"               //THIS IS THE RIGHT ORDER
  #include "platform/tcoregl.c"
  #define STB_IMAGE_IMPLEMENTATION
  #include "vendor/stb_image.h"
#endif

//Window options functions
//============================================================
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

Vector2 GetWindowSize(void)
{
  return (Vector2){(float)triggerWindow.render.width, (float)triggerWindow.render.height};
}

//Drawing functions
//============================================================
void DrawTriangle(const Vector2 v1, const Vector2 v2, const Vector2 v3, const Color color)
{
  
}

void DrawQuad(const Vector2 pos, const Vector2 size, const Color color)
{
  tglDrawQuad((Rectangle){pos.x, pos.y, size.x, size.y}, COLOR_NUMBER(color));
}

void DrawCircle(const Vector2 center, const float radius, const Color color)
{
  tglDrawCircle(center, radius, 1.0f,  COLOR_NUMBER(color));
}

void DrawTexture(const Texture texture, const Vector2 pos, const Color color)
{
  DrawTextureExtended(texture, (Rectangle){pos.x, pos.y, texture.width, texture.height}, color);  
}

void DrawTextureExtended(const Texture texture, const Rectangle data, const Color color)
{
  if (texture.id == 0) {
    TriggerLogCall(LOG_WARN, "TEXTURE-> Draw texture not valid");
    return;
  }

  tglDrawTexture((float)texture.id, data, COLOR_NUMBER(color));
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
  tglFlush2DRenderer();
#endif
}

void InputPolling(void) {

}

//Texture functions
//============================================================
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
  } else if (texture.id > 0) {
    TriggerLogCall(LOG_INFO, "TEXTURE -> [ID: %d -> %d/%d] Texture created succesfuly", texture.id, img.width, img.height);
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

  if (img.nrChannel == 1) img.nrChannel = GL_LUMINANCE;
  else if (img.nrChannel == 2) img.nrChannel = GL_LUMINANCE_ALPHA;
  else if (img.nrChannel == 3) img.nrChannel = GL_RGB;
  else if (img.nrChannel == 4) img.nrChannel = GL_RGBA;
  FreeTextData(imgData);

  if (img.data == NULL) {
    TriggerLogCall(LOG_WARN, "IMAGE -> [%s] Fail loading image from data", filepath);
  } else {
    TriggerLogCall(LOG_INFO, "IMAGE -> [%s -> %d/%d] Image loaded succesfuly", filepath, img.width, img.height);
  }

  return img;
}

//Shader functions
//============================================================
unsigned int LoadShader(const char* vShaderCode, const char* fShaderCode)
{
  int program = 0;
  int vShader = 0, fShader = 0;
  vShader = (vShaderCode == NULL || vShaderCode[0] == '\0') ?
      tglGetDefaultVertexShader() : tglCompileShader(vShaderCode, TRIGGER_VERTEX_SHADER);

  fShader = (fShaderCode == NULL || fShaderCode[0] == '\0') ?
    tglGetDefaultFragmentShader() : tglCompileShader(fShaderCode, TRIGGER_FRAGMENT_SHADER);
  
  program = tglCreateShaderProgram(vShader, fShader);
  return program;
}

void BeginShader(const unsigned int shader)
{
  tglFlushQuad2DRenderer();
  tglSetShader(shader);
}

void EndShader()
{
  tglFlushQuad2DRenderer();
  tglSetShader(tglGetDefaultShader());
}
