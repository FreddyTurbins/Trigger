//==========================================================================
//  TRIGGER 0.01
//==========================================================================

#ifndef bool
  typedef enum { false, true } bool;
#endif

typedef struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
}Color;

#define PINK (Color){200, 40, 180}

void InitWindow(const char* title, unsigned short width, unsigned short height);
bool WindowShouldClose(void);
void WindowShutdown(void);
void CloseWindow(void);

void SetBackground(Color color);
void BeginDrawing(void);
void EndDrawing(void);
