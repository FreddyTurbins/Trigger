#include "../external/glfw/include/GLFW/glfw3.h"

#if defined(_WIN32)

#endif
#if defined(__linux__)

#endif

typedef struct {
  GLFWwindow *m_window;
} PlatformData;

extern TriggerWindow triggerWindow;
PlatformData platform = {0};

int InitOpenGL(void)
{
  int result = glfwInit();
  if (result == GLFW_FALSE) { /*TRACELOG(LOG_WARNING, "GLFW: Failed to initialize GLFW");*/ return -1;}
  platform.m_window = glfwCreateWindow(triggerWindow.render.width, triggerWindow.render.height, triggerWindow.title, NULL, NULL);
  if(!platform.m_window) 
  {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(platform.m_window);
  return 0;
}

bool OpenGLShouldClose(void)
{
  return glfwWindowShouldClose(platform.m_window);
}

void OpenGLSwapScreenBuffer(void)
{
  glfwSwapBuffers(platform.m_window);
  glfwPollEvents();
}

void OpenGLCloseWindow(void)
{
  glfwTerminate();
}
