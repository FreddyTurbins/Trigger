#include "../vendor/glfw/include/GLFW/glfw3.h"
#include "../vendor/glad/glad.h"

#if defined(_WIN32)

#endif
#if defined(__linux__)

#endif

typedef struct {
  GLFWwindow *m_window;
} PlatformData;

extern TriggerWindow trigger_window;
PlatformData platform = {0};

int init_opengl(void)
{
  int result = glfwInit();
  if (result == GLFW_FALSE) { 
    trigger_log(LOG_FATAL, "Failed to initialize GLFW");
    return -1;
  }
  platform.m_window = glfwCreateWindow(trigger_window.render.width, trigger_window.render.height, trigger_window.title, NULL, NULL);
  if(!platform.m_window) 
  {
    trigger_log(LOG_FATAL, "Fail creating window");
    glfwTerminate();
    return -1;
  }
  //MAKE A ABSTRACTION TO OPENGLCONTEXT
  glfwMakeContextCurrent(platform.m_window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    trigger_log(LOG_FATAL, "Failed initializing GLAD");
    return -1;
  }
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  
  glfwSwapInterval(0);
  return 0;
}

bool opengl_should_close(void)
{
  return glfwWindowShouldClose(platform.m_window);
}

void opengl_close_window(void)
{
  glfwTerminate();
}

void opengl_swap_screen_buffer(void)
{
  glfwSwapBuffers(platform.m_window);
  glfwPollEvents();
}

void opengl_input_polling(void)
{

}
