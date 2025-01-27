//==========================================================================
//  TRIGGER 0.01
//==========================================================================
#ifndef TRIGGER_H
#define TRIGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
  #define TRAPI __declspec(dllexport)
#endif

#ifndef TRAPI
  #define TRAPI
#endif

#ifndef bool
  typedef enum { false, true } bool;
#endif

typedef enum {
  LOG_ALL,      //To activate logs
  LOG_TRACE,    //To trace internal code (function specifically)
  LOG_DEBUG,    //To display helpful information to developers
  LOG_INFO,     //To display a log information like configuration assumptions
  LOG_WARN,     //To display an odd situation, but with an automatically recovering
  LOG_ERROR,    //To display an error which is fatal to an operation or function
  LOG_FATAL,    //To display a fatal error that force a shutdown
  LOG_OFF       //To disable logs
} LogLevel;

typedef struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Color;

typedef struct Image {
  unsigned char* data;
  int width;
  int height;
  int nrChannel;
} Image;

typedef struct Texture {
  unsigned int id;
  int width;
  int height;
  int nrChannel;
} Texture;

typedef struct Vector2 {
  float x;
  float y;
} Vector2;

typedef struct Vector3 {
  float x;
  float y;
  float z;
} Vector3;

typedef struct Vector4 {
  float x;
  float y;
  float z;
  float w;
} Vector4;

typedef struct Rectangle {
  float x;
  float y;
  float width;
  float height;
} Rectangle;

typedef struct Mat4 {
  float m0, m1, m2, m3;
  float m4, m5, m6, m7;
  float m8, m9, m10, m11;
  float m12, m13, m14, m15;
} Mat4;

typedef struct Time {

} Time;

#define T_VECTOR2
#define T_VECTOR3
#define T_VECTOR4
#define T_MAT4

#define DARK_GRAY     (Color){30, 30, 30, 255}
#define PINK          (Color){200, 40, 180, 255}

//Window options related functions
//============================================================
TRAPI void InitWindow(const char* title, const unsigned short width, const unsigned short height);
TRAPI bool WindowShouldClose(void);
TRAPI void WindowShutdown(void);
TRAPI void CloseWindow(void);

//Drawing related functions
//============================================================
TRAPI void DrawTriangle(const Vector2 v1, const Vector2 v2, const Vector2 v3, const Color color);
TRAPI void DrawRectangle(const Vector2 pos, const int width, const int height, const Color color);
TRAPI void DrawRectangleExtended(const Rectangle data, const Vector2 origin, const float rotate, const Color color);
TRAPI void DrawTexture(const Texture texture, const Vector2 pos, const Color color);
TRAPI void DrawTextureExtended(const Texture texture, const Rectangle data, const Vector2 origin, const float rotate, const Color color);
TRAPI void SetBackground(const Color color);

//Texture related functions
//============================================================
TRAPI Image ReadImageFile(const char* filepath);
TRAPI Texture CreateTexture(const char* filepath);

//Frame update related fuctions
//============================================================
TRAPI void GFXUpdate(void);
TRAPI void InputPolling(void);

//Time related fuctions
//============================================================
TRAPI double GetTime(void);

//File related functions
//============================================================
TRAPI char* ReadTextFile(const char* filepath, unsigned int* readLen);
TRAPI void FreeTextData(unsigned char* data);

//Util related functions
//============================================================
TRAPI void TriggerLogCall(const int logLevel, const char* fmt, ...);
TRAPI void SetTriggerLogLevel(const int logLevel);
#endif
