//==========================================================================
//  TRIGGER 0.01
//==========================================================================

#ifndef bool
  typedef enum { false, true } bool;
#endif

void InitWindow(const char* title, unsigned short width, unsigned short height);
bool WindowShouldClose(void);
void WindowShutdown(void);
void BeginDrawing(void);
void EndDrawing(void);
void CloseWindow(void);
