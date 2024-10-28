//==========================================================================
//  TRIGGER 0.01
//==========================================================================

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

#define DARK_GRAY     (Color){30, 30, 30, 255}
#define PINK          (Color){200, 40, 180, 255}

//Window options related fuctions
//============================================================
void InitWindow(const char* title, const unsigned short width, const unsigned short height);
bool WindowShouldClose(void);
void WindowShutdown(void);
void CloseWindow(void);

//Drawing functions
//============================================================
void SetBackground(const Color color);

//Frame update fuctions
//============================================================
void GFXUpdate(void);
void InputPolling(void);

//File functions
//============================================================
char* ReadTextFile(const char* filepath);

//Util functions
//============================================================
void TriggerLogCall(const int logLevel, const char* fmt, ...);
void SetTriggerLogLevel(const int logLevel);
