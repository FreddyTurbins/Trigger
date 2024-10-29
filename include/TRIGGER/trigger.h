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

typedef struct Vector2 {
  float x;
  float y;
} Vector2;

#define DARK_GRAY     (Color){30, 30, 30, 255}
#define PINK          (Color){200, 40, 180, 255}

//Window options related functions
//============================================================
TRAPI void InitWindow(const char* title, const unsigned short width, const unsigned short height);
TRAPI bool WindowShouldClose(void);
TRAPI void WindowShutdown(void);
TRAPI void CloseWindow(void);

//Drawing functions
//============================================================
TRAPI void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);
TRAPI void SetBackground(const Color color);

//Frame update fuctions
//============================================================
TRAPI void GFXUpdate(void);
TRAPI void InputPolling(void);

//File functions
//============================================================
TRAPI char* ReadTextFile(const char* filepath);

//Util functions
//============================================================
TRAPI void TriggerLogCall(const int logLevel, const char* fmt, ...);
TRAPI void SetTriggerLogLevel(const int logLevel);
#endif
