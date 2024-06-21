/*************************************************************************************************
*
*     Trigger open-gl abstraction API
*
*************************************************************************************************/

#ifndef TGL_H
#define TGL_H
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

void TESetBackground(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void TEClearScreenBuffer(void);

void TESetBackground(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  float red = (float)r/255;
  float green = (float)g/255;
  float blue = (float)b/255;
  float alpha = (float)a/255;

  glClearColor(red, green, blue, alpha);
  TEClearScreenBuffer();
}

void TEClearScreenBuffer(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
#endif
