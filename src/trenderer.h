#include "tmath.h"

//2D Renderer related functions
//============================================================
void tglInit2DRenderer(void);
void tglStartBatch(void);
void tglFlush2DRenderer(void);
void tglFlushQuad2DRenderer(void);

//2D Renderer drawing related functions
//============================================================
void tglDrawQuad(const Rectangle data, const unsigned long color);
void tglDrawCircle(const Vector2 center, const float radius, const float thicness, const unsigned long color);
void tglDrawTexture(const float texture, const Rectangle data, const unsigned long color);
