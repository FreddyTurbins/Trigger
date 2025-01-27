#include "../vendor/glfw/include/GLFW/glfw3.h"
#include "../vendor/glad/glad.h"

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
  if (result == GLFW_FALSE) { 
    TriggerLogCall(LOG_FATAL, "Failed to initialize GLFW");
    return -1;
  }
  platform.m_window = glfwCreateWindow(triggerWindow.render.width, triggerWindow.render.height, triggerWindow.title, NULL, NULL);
  if(!platform.m_window) 
  {
    TriggerLogCall(LOG_FATAL, "Fail creating window");
    glfwTerminate();
    return -1;
  }
  //MAKE A ABSTRACTION TO OPENGLCONTEXT
  glfwMakeContextCurrent(platform.m_window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    TriggerLogCall(LOG_FATAL, "Failed initializing GLAD");
    return -1;
  }
  //glViewport(0, 0, triggerWindow.render.width, triggerWindow.render.height);
  return 0;
}

bool OpenGLShouldClose(void)
{
  return glfwWindowShouldClose(platform.m_window);
}

void OpenGLCloseWindow(void)
{
  glfwTerminate();
}

void OpenGLSwapScreenBuffer(void)
{
  glfwSwapBuffers(platform.m_window);
  glfwPollEvents();
}

void OpenGLInputPolling(void)
{

}

double GetTime(void)
{
  return glfwGetTime();
}
